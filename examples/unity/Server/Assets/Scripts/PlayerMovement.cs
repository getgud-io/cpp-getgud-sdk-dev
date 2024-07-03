using Riptide;
using UnityEngine;
using GetgudSDK;
using System;
using UnityEngine.UIElements;

[RequireComponent(typeof(CharacterController))]
public class PlayerMovement : MonoBehaviour
{
    [SerializeField] private Player player;
    [SerializeField] private CharacterController controller;
    [SerializeField] private Transform camProxy;
    [SerializeField] private float gravity;
    [SerializeField] private float movementSpeed;
    [SerializeField] private float jumpHeight;

    private float gravityAcceleration;
    private float moveSpeed;
    private float jumpSpeed;

    private bool[] inputs;
    private float yVelocity;
    private bool didTeleport;

    private void OnValidate()
    {
        if (controller == null)
            controller = GetComponent<CharacterController>();
        if (player == null)
            player = GetComponent<Player>();

        Initialize();
    }

    private void Start()
    {
        Initialize();

        inputs = new bool[6];
    }

    private void FixedUpdate()
    {
        if (!player.IsAlive)
            return;

        Vector2 inputDirection = Vector2.zero;
        if (inputs[0])
            inputDirection.y += 1;
        
        if (inputs[1])
            inputDirection.y -= 1;

        if (inputs[2])
            inputDirection.x -= 1;

        if (inputs[3])
            inputDirection.x += 1;

        Move(inputDirection, inputs[4], inputs[5]);
    }

    private void Initialize()
    {
        gravityAcceleration = gravity * Time.fixedDeltaTime * Time.fixedDeltaTime;
        moveSpeed = movementSpeed * Time.fixedDeltaTime;
        jumpSpeed = Mathf.Sqrt(jumpHeight * -2f * gravityAcceleration);
    }

    public void Enabled(bool value)
    {
        enabled = value;
        controller.enabled = value;
    }

    public void Teleport(Vector3 toPosition)
    {
        bool isEnabled = controller.enabled;
        controller.enabled = false;
        transform.position = toPosition;
        controller.enabled = isEnabled;

        didTeleport = true;
    }

    private void Move(Vector2 inputDirection, bool jump, bool sprint)
    {
        Vector3 moveDirection = Vector3.Normalize(camProxy.right * inputDirection.x + Vector3.Normalize(FlattenVector3(camProxy.forward)) * inputDirection.y);
        moveDirection *= moveSpeed;

        if (sprint)
            moveDirection *= 2f;

        if (controller.isGrounded)
        {
            yVelocity = 0f;
            if (jump)
                yVelocity = jumpSpeed;
        }
        yVelocity += gravityAcceleration;

        moveDirection.y = yVelocity;
        controller.Move(moveDirection);

        SendMovement();
    }

    private Vector3 FlattenVector3(Vector3 vector)
    {
        vector.y = 0;
        return vector;
    }

    public void SetInput(bool[] inputs, Vector3 forward)
    {
        this.inputs = inputs;
        camProxy.forward = forward;
    }

    private void SendMovement()
    {
        if (NetworkManager.Singleton.CurrentTick % 2 != 0)
            return;

        Message message = Message.Create(MessageSendMode.Unreliable, ServerToClientId.playerMovement);
        message.AddUShort(player.Id);
        message.AddUShort(NetworkManager.Singleton.CurrentTick);
        message.AddBool(didTeleport);
        message.AddVector3(transform.position);
        message.AddVector3(camProxy.forward);
        NetworkManager.Singleton.Server.SendToAll(message);

        // Convert camProxy.forward to pitch, yaw, roll
        Vector3 forward = camProxy.forward;
        Vector3 up = camProxy.up;
        Vector3 right = camProxy.right;

        // Calculate pitch, yaw, roll
        float pitch = Mathf.Asin(forward.y) * Mathf.Rad2Deg;
        float yaw = Mathf.Atan2(forward.x, forward.z) * Mathf.Rad2Deg;
        float roll = Mathf.Atan2(right.y, up.y) * Mathf.Rad2Deg;

        Debug.Log($"Getgud: Player {player.Username} moved {transform.position.ToString()} with pitch: {pitch}, yaw: {yaw}, roll: {roll}");
        BaseActionData positionBaseActionData = new BaseActionData
        {
            actionTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeSeconds(),
            matchGuid = NetworkManager.Singleton.MatchGuid,
            playerGuid = "example-player-guid"
        };

        // Create PositionF for position action
        PositionF currentPosition = new PositionF { X = transform.position.x, Y = transform.position.y, Z = transform.position.z };
        // Create RotationF for position action
        RotationF currentRotation = new RotationF { Yaw = yaw, Pitch = pitch, Roll = roll };

        // Create SendPositionActionInfo
        SendPositionActionInfo positionInfo = new SendPositionActionInfo
        {
            baseData = positionBaseActionData,
            position = currentPosition,
            rotation = currentRotation
        };

        // Send the position action
        int positionResult = Methods.SendPositionAction(positionInfo);

        didTeleport = false;
    }
}
