#pragma once

#include "BitStream.h"

/// These enumerations are used to describe when packets are delivered.
enum PacketPriority
{
	SYSTEM_PRIORITY,   /// \internal Used by RakNet to send above-high priority messages.
	HIGH_PRIORITY,   /// High priority messages are send before medium priority messages.
	MEDIUM_PRIORITY,   /// Medium priority messages are send before low priority messages.
	LOW_PRIORITY,   /// Low priority messages are only sent when no other messages are waiting.
	NUMBER_OF_PRIORITIES
};

/// These enumerations are used to describe how packets are delivered.
/// \note  Note to self: I write this with 3 bits in the stream.  If I add more remember to change that
enum PacketReliability
{
	UNRELIABLE = 6,   /// Same as regular UDP, except that it will also discard duplicate datagrams.  RakNet adds (6 to 17) + 21 bits of overhead, 16 of which is used to detect duplicate packets and 6 to 17 of which is used for message length.
	UNRELIABLE_SEQUENCED,  /// Regular UDP with a sequence counter.  Out of order messages will be discarded.  This adds an additional 13 bits on top what is used for UNRELIABLE.
	RELIABLE,   /// The message is sent reliably, but not necessarily in any order.  Same overhead as UNRELIABLE.
	RELIABLE_ORDERED,   /// This message is reliable and will arrive in the order you sent it.  Messages will be delayed while waiting for out of order messages.  Same overhead as UNRELIABLE_SEQUENCED.
	RELIABLE_SEQUENCED /// This message is reliable and will arrive in the sequence you sent it.  Out or order messages will be dropped.  Same overhead as UNRELIABLE_SEQUENCED.
};

#pragma pack(push, 1)

struct PlayerID
{
	unsigned int binaryAddress;
	unsigned short port;
};

/// All RPC functions have the same parameter list - this structure.
struct RPCParameters
{
	/// The data from the remote system
	unsigned char *input;

	/// How many bits long \a input is
	unsigned int numberOfBitsOfData;

	/// Which system called this RPC
	PlayerID sender;

	/// Which instance of RakPeer (or a derived RakServer or RakClient) got this call
	void *recipient;

	/// You can return values from RPC calls by writing them to this BitStream.
	/// This is only sent back if the RPC call originally passed a BitStream to receive the reply.
	/// If you do so and your send is reliable, it will block until you get a reply or you get disconnected from the system you are sending to, whichever is first.
	/// If your send is not reliable, it will block for triple the ping time, or until you are disconnected, or you get a reply, whichever is first.
	RakNet::BitStream *replyToSender;
};
#pragma pack(pop)

/// \sa NetworkIDGenerator.h
typedef unsigned char UniqueIDType;
typedef unsigned short PlayerIndex;
typedef unsigned char RPCIndex;
const int MAX_RPC_MAP_SIZE = ((RPCIndex)-1) - 1;
const int UNDEFINED_RPC_INDEX = ((RPCIndex)-1);

/// First byte of a network message
typedef unsigned char MessageID;

typedef unsigned int RakNetTime;
typedef long long RakNetTimeNS;

struct RakNetStatisticsStruct
{
	///  Number of Messages in the send Buffer (high, medium, low priority)
	unsigned messageSendBuffer[NUMBER_OF_PRIORITIES];
	///  Number of messages sent (high, medium, low priority)
	unsigned messagesSent[NUMBER_OF_PRIORITIES];
	///  Number of data bits used for user messages
	unsigned messageDataBitsSent[NUMBER_OF_PRIORITIES];
	///  Number of total bits used for user messages, including headers
	unsigned messageTotalBitsSent[NUMBER_OF_PRIORITIES];

	///  Number of packets sent containing only acknowledgements
	unsigned packetsContainingOnlyAcknowlegements;
	///  Number of acknowledgements sent
	unsigned acknowlegementsSent;
	///  Number of acknowledgements waiting to be sent
	unsigned acknowlegementsPending;
	///  Number of acknowledgements bits sent
	unsigned acknowlegementBitsSent;

	///  Number of packets containing only acknowledgements and resends
	unsigned packetsContainingOnlyAcknowlegementsAndResends;

	///  Number of messages resent
	unsigned messageResends;
	///  Number of bits resent of actual data
	unsigned messageDataBitsResent;
	///  Total number of bits resent, including headers
	unsigned messagesTotalBitsResent;
	///  Number of messages waiting for ack (// TODO - rename this)
	unsigned messagesOnResendQueue;

	///  Number of messages not split for sending
	unsigned numberOfUnsplitMessages;
	///  Number of messages split for sending
	unsigned numberOfSplitMessages;
	///  Total number of splits done for sending
	unsigned totalSplits;

	///  Total packets sent
	unsigned packetsSent;

	///  Number of bits added by encryption
	unsigned encryptionBitsSent;
	///  total bits sent
	unsigned totalBitsSent;

	///  Number of sequenced messages arrived out of order
	unsigned sequencedMessagesOutOfOrder;
	///  Number of sequenced messages arrived in order
	unsigned sequencedMessagesInOrder;

	///  Number of ordered messages arrived out of order
	unsigned orderedMessagesOutOfOrder;
	///  Number of ordered messages arrived in order
	unsigned orderedMessagesInOrder;

	///  Packets with a good CRC received
	unsigned packetsReceived;
	///  Packets with a bad CRC received
	unsigned packetsWithBadCRCReceived;
	///  Bits with a good CRC received
	unsigned bitsReceived;
	///  Bits with a bad CRC received
	unsigned bitsWithBadCRCReceived;
	///  Number of acknowledgement messages received for packets we are resending
	unsigned acknowlegementsReceived;
	///  Number of acknowledgement messages received for packets we are not resending
	unsigned duplicateAcknowlegementsReceived;
	///  Number of data messages (anything other than an ack) received that are valid and not duplicate
	unsigned messagesReceived;
	///  Number of data messages (anything other than an ack) received that are invalid
	unsigned invalidMessagesReceived;
	///  Number of data messages (anything other than an ack) received that are duplicate
	unsigned duplicateMessagesReceived;
	///  Number of messages waiting for reassembly
	unsigned messagesWaitingForReassembly;
	///  Number of messages in reliability output queue
	unsigned internalOutputQueueSize;
	///  Current bits per second
	double bitsPerSecond;
	///  connection start time
	RakNetTime connectionStartTime;
};

#pragma pack(push, 1)
/// This represents a user message from another system.
struct Packet
{
	/// Server only - this is the index into the player array that this playerId maps to
	PlayerIndex playerIndex;

	/// The system that send this packet.
	PlayerID playerId;

	/// The length of the data in bytes
	/// \deprecated You should use bitSize.
	unsigned int length;

	/// The length of the data in bits
	unsigned int bitSize;

	/// The data from the sender
	unsigned char* data;

	/// @internal
	/// Indicates whether to delete the data, or to simply delete the packet.
	bool deleteData;
};
#pragma pack(pop)

const PlayerID UNASSIGNED_PLAYER_ID =
{
	0xFFFFFFFF, 0xFFFF
};

#ifdef WIN32

class CCRakServer
{
public:
	~CCRakServer();
	virtual void _0();
	virtual bool Start(unsigned short AllowedPlayers, unsigned int depreciated, int threadSleepTimer, unsigned short port, const char *forceHostAddress = 0) = 0; // 4
	virtual void DisableSecurity(void); // 8
	virtual void SetPassword(const char *_password);	// 12
	virtual bool HasPassword(void);	// 16
	virtual void Disconnect(unsigned int blockDuration, unsigned char orderingChannel = 0);	// 20
	virtual bool Send_ASD(const char *data, const int length, int priority, int reliability, char orderingChannel, PlayerID playerId, bool broadcast); // 24
	virtual bool Send(RakNet::BitStream* parameters, int priority, int reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast);	// 28
	virtual void _20(); // 
	virtual void _24();
	virtual Packet* Receive(void);
	virtual void CloseConnection(const PlayerID playerId); // 0x2C
	virtual void DeallocatePacket(Packet* asd);
	virtual void SetAllowedPlayers(unsigned short numberAllowed); // 0x34
	virtual unsigned short GetAllowedPlayers(void) const;
	virtual unsigned short GetConnectedPlayers(void);
	virtual void GetPlayerIPFromID(const PlayerID playerId, char returnValue[22], unsigned short *port);
	virtual void PingPlayer(const PlayerID playerId);
	virtual int GetAveragePing(const PlayerID playerId);
	virtual int GetLastPing(const PlayerID playerId); // 4C
	virtual int GetLowestPing(const PlayerID playerId);
	virtual void _54();
	virtual void _58();
	virtual void _5C();
	virtual void _60();
	virtual void _64();
	virtual void _68();
	virtual void _6C();
	virtual void _70();
	virtual void RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms));	// 74
	virtual void RegisterClassMemberRPC(int* uniqueID, void *functionPointer);	// 78
	virtual void UnregisterAsRemoteProcedureCall(int* uniqueID);	// 7C
	virtual bool RPC(int* uniqueID, RakNet::BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp); // 80
	virtual void _84();
	virtual void _88();
	virtual void _8C();
	virtual void _90();
	virtual void _94();
	virtual void _98();
	virtual void _9C();
	virtual void _A0();
	virtual void _A4();
	virtual void _A8();
	virtual void _AC();
	virtual void _B0();
	virtual void _B4();
	virtual void _B8();
	virtual void _BC();
	virtual void _C0();
	virtual void _C4();
	virtual void _C8();
	virtual void _CC();
	virtual const char* GetLocalIP(unsigned int index);
	virtual PlayerID GetInternalID(void) const;
	virtual void PushBackPacket(Packet *packet, bool pushAtHead);
	virtual void SetRouterInterface(void *routerInterface);
	virtual void RemoveRouterInterface(void *routerInterface);
	virtual int GetIndexFromPlayerID(PlayerID playerId); // E4
	virtual PlayerID GetPlayerIDFromIndex(int index); // E8
	virtual void AddToBanList(const char *IP, unsigned int milliseconds = 0);
	virtual void RemoveFromBanList(const char *IP);
	virtual void ClearBanList(void);
	virtual bool IsBanned(const char *IP);
	virtual bool IsActivePlayerID(const PlayerID playerId);
	virtual void SetTimeoutTime(RakNetTime timeMS, const PlayerID target);
	virtual bool SetMTUSize(int size);
	virtual int GetMTUSize(void) const;
	virtual void AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength);
	virtual RakNetStatisticsStruct * const GetStatistics(const PlayerID playerId);
	virtual void ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance);
};

#else

class CCRakServer
{
public:
	~CCRakServer();
	virtual bool _0();
	virtual void _4();
	virtual bool Start(unsigned short AllowedPlayers, unsigned int depreciated, int threadSleepTimer, unsigned short port, const char *forceHostAddress = 0) = 0;
	virtual void _C();
	virtual void _10();
	virtual void _14();
	virtual void _18();
	virtual void _1C();
	virtual bool SendGECI(RakNet::BitStream* parameters, int priority, int reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast);
	virtual bool Send(RakNet::BitStream* parameters, int priority, int reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast);
	virtual void _28();
	virtual void _2C();
	virtual void _30();
	virtual void _34();
	virtual void SetAllowedPlayers(unsigned short numberAllowed);
	virtual void _3C();
	virtual void _40();
	virtual void _44();
	virtual void _48();
	virtual void _4C();
	virtual int GetLastPing(const PlayerID playerId); // 0x50
	virtual void _54();
	virtual void _58();
	virtual void _5C();
	virtual const char* GetLocalIP(unsigned int index);
	virtual PlayerID GetInternalID(void) const;
	virtual void PushBackPacket(Packet *packet, bool pushAtHead);
	virtual void SetRouterInterface(void *routerInterface);
	virtual void RemoveRouterInterface(void *routerInterface);
	virtual void UnregisterAsRemoteProcedureCall_BAD(int* uniqueID);
	virtual void RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms)); // 120
	virtual void RegisterClassMemberRPC(int* uniqueID, void *functionPointer); // 124
	virtual void UnregisterAsRemoteProcedureCall(int* uniqueID); // 128
	virtual void UnregisterAsRemoteProcedureCall_4(int* uniqueID);
	virtual void UnregisterAsRemoteProcedureCall_5(int* uniqueID);
	virtual bool RPC(int* uniqueID, RakNet::BitStream* parameters, int priority, int reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp);
	virtual void _90();
	virtual void _94();
	virtual void _98();
	virtual void _9C();
	virtual void _A0();
	virtual void _A4();
	virtual void _A8();
	virtual void _AC();
	virtual void _B0();
	virtual void _B4();
	virtual void _B8();
	virtual void _BC();
	virtual void _C0();
	virtual void _C4();
	virtual void _C8();
	virtual void _CC();
	virtual void _D0();
	virtual void _D4();
	virtual void _D8();
	virtual void _DC();
	virtual void _E0();
	virtual void _E8();
	virtual int GetIndexFromPlayerID(PlayerID playerId); // EC
	virtual PlayerID GetPlayerIDFromIndex(int index); // F0 - 240 - 244
	virtual void AddToBanList(const char *IP, unsigned int milliseconds = 0); // 244 - 248
	virtual void RemoveFromBanList(const char *IP); // 248 - 252
	virtual void ClearBanList(void);
	virtual bool IsBanned(const char *IP);
	virtual bool IsActivePlayerID(const PlayerID playerId);
	virtual void SetTimeoutTime(RakNetTime timeMS, const PlayerID target);
	virtual bool SetMTUSize(int size);
	virtual int GetMTUSize(void) const;
	virtual void AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength);
	virtual RakNetStatisticsStruct * const GetStatistics(const PlayerID playerId);
	virtual void ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance);
};
#endif