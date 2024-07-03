using Riptide;
using Riptide.Utils;
using UnityEngine;
using GetgudSDK;
using System;

public enum ServerToClientId : ushort
{
    sync = 1,
    activeScene,
    playerSpawned,
    playerMovement,
    playerHealthChanged,
    playerActiveWeaponUpdated,
    playerAmmoChanged,
    playerDied,
    playerRespawned,
    projectileSpawned,
    projectileMovement,
    projectileCollided,
    projectileHitmarker,
}

public enum ClientToServerId : ushort
{
    name = 1,
    input,
    switchActiveWeapon,
    primaryUse,
    reload,
}

public class NetworkManager : MonoBehaviour
{
    private static NetworkManager _singleton;
    public static NetworkManager Singleton
    {
        get => _singleton;
        private set
        {
            if (_singleton == null)
                _singleton = value;
            else if (_singleton != value)
            {
                Debug.Log($"{nameof(NetworkManager)} instance already exists, destroying duplicate!");
                Destroy(value);
            }
        }
    }

    public Server Server { get; private set; }
    public ushort CurrentTick { get; private set; } = 0;

    [SerializeField] private ushort port;
    [SerializeField] private ushort maxClientCount;

    public string GameGuid { get; private set; }  // Public property to access gameGuid
    public string MatchGuid { get; private set; }  // Public property to access matchGuid

    private void Awake()
    {
        Singleton = this;
    }

    private void Start()
    {
        Application.targetFrameRate = 60;

#if UNITY_EDITOR
        RiptideLogger.Initialize(Debug.Log, Debug.Log, Debug.LogWarning, Debug.LogError, false);
#else
        System.Console.Title = "Server";
        System.Console.Clear();
        Application.SetStackTraceLogType(UnityEngine.LogType.Log, StackTraceLogType.None);
        RiptideLogger.Initialize(Debug.Log, true);
#endif

        Server = new Server();
        Server.ClientConnected += NewPlayerConnected;
        Server.ClientDisconnected += PlayerLeft;

        Server.Start(port, maxClientCount);

        GameLogic.Singleton.LoadScene(1);

        // Initialize GetgudSDK
        int status = GetgudSDK.Methods.Init();
        Debug.Log($"GetgudSDK Init status: {status}");

        StartGameAndMatch();
    }

    private void FixedUpdate()
    {
        Server.Update();

        if (CurrentTick % 200 == 0)
            SendSync();

        CurrentTick++;
    }

    private void OnApplicationQuit()
    {
        Server.Stop();
    }

    private void NewPlayerConnected(object sender, ServerConnectedEventArgs e)
    {
        GameLogic.Singleton.PlayerCountChanged(e.Client.Id);
    }

    private void PlayerLeft(object sender, ServerDisconnectedEventArgs e)
    {
        if (Player.list.TryGetValue(e.Client.Id, out Player player))
            Destroy(player.gameObject);

        EndGame();
    }

    private void SendSync()
    {
        Message message = Message.Create(MessageSendMode.Unreliable, (ushort)ServerToClientId.sync);
        message.AddUShort(CurrentTick);

        Server.SendToAll(message);
    }

    private void StartGameAndMatch()
    {
        StartGameInfo gameInfo = new StartGameInfo
        {
            TitleId = 111, // Use your actual TitleId
            PrivateKey = "private-key", // Use your actual private key
            ServerGuid = "example-server-guid",
            GameMode = "example-game-mode",
            ServerLocation = "example-server-location"
        };

        int gameResult = GetgudSDK.Methods.StartGame(gameInfo, out string gameGuid);
        Debug.Log($"StartGame result: {gameResult}, gameGuid: {gameGuid}");

        if (gameResult == 0)
        {
            GameGuid = gameGuid;

            StartMatchInfo matchInfo = new StartMatchInfo
            {
                gameGuid = gameGuid,
                matchMode = "example-match-mode",
                mapName = "example-map-name"
            };

            int matchResult = GetgudSDK.Methods.StartMatch(matchInfo, out string matchGuid);
            Debug.Log($"StartMatch result: {matchResult}, matchGuid: {matchGuid}");

            if (matchResult == 0)
            {
                MatchGuid = matchGuid;
            }
            else
            {
                Debug.LogError("Failed to start match.");
            }
        }
        else
        {
            Debug.LogError("Failed to start game.");
        }
    }

    private void EndGame()
    {
        int endGameResult = GetgudSDK.Methods.MarkEndGame(GameGuid);
        Debug.Log($"EndGame result: {endGameResult}");
    }
}