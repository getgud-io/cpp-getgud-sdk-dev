using Benchmarking;
using UnityEngine;
using GetgudSDK;
using System;
using UnityEngine.Windows;
#if ENABLE_INPUT_SYSTEM
using UnityEngine.InputSystem;
#endif

namespace StarterAssets
{
    public class StarterAssetsInputs : MonoBehaviour
	{
		[Header("Character Input Values")]
		public Vector2 move;
		public Vector2 look;
		public bool jump;
		public bool sprint;
        public string privateKey = "a6effeb0-b3b6-11ee-a523-25e8feeae2d8";
        public int titleId = 111;
        public string gameGuid;
        public string matchGuid;

        [Header("Movement Settings")]
		public bool analogMovement;

		[Header("Mouse Cursor Settings")]
		public bool cursorLocked = true;
		public bool cursorInputForLook = true;

		public PlayerManager CameraManager;
		
		private bool m_IgnoreInput;
		

		private static bool m_FocusActionsSetUp;

		private void Start()
		{
            if (!m_FocusActionsSetUp)
			{
#if UNITY_EDITOR
				var ignoreInput = new InputAction(binding: "/Keyboard/escape");
				ignoreInput.performed += context => m_IgnoreInput = true;
				ignoreInput.Enable();

				var enableInput = new InputAction(binding: "/Mouse/leftButton");
				enableInput.performed += context => m_IgnoreInput = false;
				enableInput.Enable();
#endif
				
				var touchFocus = new InputAction(binding: "<pointer>/press");
				touchFocus.performed += context => CameraManager.NotifyPlayerMoved();
				touchFocus.Enable();
				
				m_FocusActionsSetUp = true;

				//Initialize the SDK and create Game and Match

                int status = GetgudSDK.Methods.Init();

                // Initialize input and output data for a Game

                GetgudSDK.StartGameInfo gameInfo;
                gameInfo.ServerGuid = "123";
                gameInfo.PrivateKey = privateKey;
                gameInfo.TitleId = titleId;
                gameInfo.GameMode = "gameMode";
                gameInfo.ServerLocation = "UK";

                // Start a Game:
                status = GetgudSDK.Methods.StartGame(gameInfo, out gameGuid);

                // Initialize input and output data for a Match

                GetgudSDK.StartMatchInfo matchInfo;
                matchInfo.gameGuid = gameGuid;
                matchInfo.mapName = "map";
                matchInfo.matchMode = "matchMode";

                // Start a Match of the Game:
                status = GetgudSDK.Methods.StartMatch(matchInfo, out matchGuid);


                //Create Spawn action because we have a player now

                GameObject mainCamera = GameObject.FindGameObjectWithTag("MainCamera");

                // Initialize input data for an Action
                GetgudSDK.SendSpawnActionInfo SpawnInfo;
                SpawnInfo.baseData.matchGuid = matchGuid;
                SpawnInfo.baseData.playerGuid = "player-1";
                SpawnInfo.baseData.actionTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
                SpawnInfo.characterGuid = name;
                SpawnInfo.initialHealth = 100;
                SpawnInfo.teamId = 1;
                SpawnInfo.position = new GetgudSDK.PositionF { X = transform.position.x, Y = transform.position.y, Z = transform.position.z };
                SpawnInfo.rotation = new GetgudSDK.RotationF { Roll = mainCamera.transform.rotation.eulerAngles.z, Pitch = mainCamera.transform.rotation.eulerAngles.x, Yaw = mainCamera.transform.rotation.eulerAngles.y };


                // Send the action to the SDK:
                GetgudSDK.Methods.SendSpawnAction(SpawnInfo);

                SpawnInfo.baseData.playerGuid = "player-2";

                // Send the action to the SDK:
                GetgudSDK.Methods.SendSpawnAction(SpawnInfo);
            }
		}

		private void OnDestroy()
		{
			if (gameGuid.Length > 0)
			{
                GetgudSDK.Methods.MarkEndGame(gameGuid);
				gameGuid = "";
            }

            m_FocusActionsSetUp = false;
        }


#if ENABLE_INPUT_SYSTEM
		public void OnMove(InputValue value)
		{
			if (m_IgnoreInput)
			{
				MoveInput(Vector2.zero);
				return;
			}
			
			if (CameraManager != null)
			{
				CameraManager.NotifyPlayerMoved();
			}
			MoveInput(value.Get<Vector2>());
		}

		public void OnLook(InputValue value)
		{
			if (m_IgnoreInput)
			{
				LookInput(Vector2.zero);
				return;
			}
			
			if (CameraManager != null)
			{
				CameraManager.NotifyPlayerMoved();
			}
			if(cursorInputForLook)
			{
				LookInput(value.Get<Vector2>());
			}
		}

		public void OnJump(InputValue value)
		{
			if (CameraManager != null)
			{
				CameraManager.NotifyPlayerMoved();
			}
			JumpInput(value.isPressed);
		}

		public void OnSprint(InputValue value)
		{
			if (CameraManager != null)
			{
				CameraManager.NotifyPlayerMoved();
			}
			SprintInput(value.isPressed);
		}
#endif


		public void MoveInput(Vector2 newMoveDirection)
		{
			move = newMoveDirection;
        } 

		public void LookInput(Vector2 newLookDirection)
		{ 
			look = newLookDirection;
        }

		public void JumpInput(bool newJumpState)
		{
			jump = newJumpState;
		}

		public void SprintInput(bool newSprintState)
		{
			sprint = newSprintState;
		}
		
		private void OnApplicationFocus(bool hasFocus)
		{
			SetCursorState(cursorLocked);
			m_IgnoreInput = !hasFocus;
		}

		private void SetCursorState(bool newState)
		{
			if (PerformanceTest.RunningBenchmark)
				return;

			Cursor.lockState = newState ? CursorLockMode.Locked : CursorLockMode.None;
		}
	}
	
}