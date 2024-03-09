---

title: 进程数据读取，我好像找到了某个FPS一哥的软肋？

tags:    

- "游戏"
- "hack"

categories:

- "fun"

thumbnail: https://s2.loli.net/2024/03/10/ioGrUJAHF2RsLdh.png

---

# 写在前面

无聊，翻油管看到这样一个视频[I Bought CHEATING GLASSES for CS2 (Worlds First?!) (youtube.com)](https://www.youtube.com/watch?v=a93ps-etVNc&t=243s)

![image-20240310023902255](https://s2.loli.net/2024/03/10/oMGxmJODp2wF9U6.png)

在感慨外挂在这片土地上肆虐的同时，也不禁好奇它们是如何做到的。

也有段时间没有打CS2了，不如就此下载回来研究一下，为什么外挂使用者们能获取到实时游戏数据并滥用它。

遂上某网站(为避免不良影响，此处不提供信息)翻找，很快就获取了作弊软件本身及其源码。

另人啼笑皆非的是，创作者对软件是开源的，而竟被剽窃者送至某多多以35元的价格卖出了10万份。

本文章只提供部分代码以及核心原理作学习使用，请勿用作不良用途，后果自负！

# 说说CS2

![image-20240310024522523](https://s2.loli.net/2024/03/10/bHosTE6P8eYNfBO.png)

Steam上有句话说得好：

> 二十多年来，在全球数百万玩家的共同铸就下，Counter-Strike 提供了精湛绝伦的竞技体验。而如今，CS 传奇的下一章即将揭开序幕，那就是 Counter-Strike 2。

我们就来看看他揭开了怎样的精湛绝伦的序幕。

对于作弊者是如何获取实时游戏数据的，其实就一句话：**用户能通过cs2进程用客户端动态链接库id拿到包括自己在内的全部玩家信息**

是的，你没听错，也许这是为了对每个用户都更好地表现赛场的状态，我试着还原一下场景，不一定正确但一定能形容这种情况：

在某一个tick内，CT在A点拆包，你从大坑瞄准他用大狙开火了。此时CT和你的实体都存在于你的计算机上，在你的计算机上你杀死了他，此时才提交服务器，如果在他的计算机上他躲过了这次子弹，那可能要看你们谁上传到服务器更快了。

而全部玩家信息保存在服务端这种相对安全的情况则是另一种场景：你在大坑开枪了，你的状态上传到服务器，于此同时CT拆包的状态也上传的服务器，你是否击中他则由服务器判断。很显然这种所有数据(除自身)交给服务器保管才是安全的，但不知道为什么v社采用了第一种。

# 详细过程

**不得不说，如果你有歪点子，只是想要白嫖一份外挂的话，凭我放出的代码量你现在已经可以关闭网页了。**

想学到一些东西或者单纯觉得有趣的朋友可以继续往下看。

首先要做的自然是连接进程

```cpp
auto ProcessStatus = ProcessMgr.Attach("cs2.exe");
```

以及拿到一切数据的起始地址,client.dll的地址

```cpp
DWORD64 ClientDLL = reinterpret_cast<DWORD64>(ProcessMgr.GetProcessModuleHandle("client.dll"));
if (ClientDLL == 0)
	return false;
```

起始地址有了，后面接的是一大块数据块，你该怎么分别获取不同数据的内容呢

用offset(偏移)，偏移是每个特定数据与起始地址的相对距离，起始地址加上特定偏移就可以读到特定数据了。

如要获取实体列表的位置

```cpp
this->Address.EntityList = GetClientDLLAddress() + Offset::EntityList;
```

现在问题只剩下一个了，就是不同offset的求取，这就和cs2的数据存放方式有关了，cs2数据的存放都是"签名+数据"(还有些处理在里面，我不说那么细了)

那么只要在一个范围内查到签名的位置，也就查到了数据的位置，便可以用这个位置减去起始位置得到偏移了。下次再查找就不用签名配对而是直接偏移求址了

```cpp
TempAddress = SearchOffsets(Offset::Signatures::EntityList, ClientDLL);
if (TempAddress == 0)
	return false;
Offset::EntityList = TempAddress - ClientDLL;
```

在我放出的偏移命名空间内你可以看到所有可以拿到的数据以及某个数据块下的小数据块相对这个数据块的偏移(如Entity下的Health或TeamID等)，但是**它们的内容我都做过处理，不用寄希望于直接使用**。有心者可以联系我或者自己读取整个链接库的内存逐一做分析得到。

```cpp
namespace Offset
{
	inline DWORD ForceJump;
	inline DWORD ForceCrouch;
	inline DWORD ForceForward;
	inline DWORD ForceLeft;
	inline DWORD ForceRight;

	inline DWORD EntityList;
	inline DWORD Matrix;
	inline DWORD ViewAngle;
	inline DWORD LocalPlayerController;
	inline DWORD LocalPlayerPawn;
	inline DWORD GlobalVars;
	inline DWORD InventoryServices;
	inline DWORD PlantedC4;
	inline DWORD InputSystem;
	inline DWORD Sensitivity;
	inline DWORD Pointer;

	struct
	{
		DWORD Health = unknown;
		DWORD TeamID = unknown;
		DWORD IsAlive = unknown;
		DWORD PlayerPawn = unknown;
		DWORD iszPlayerName = unknown;
		DWORD EnemySensor = unknown;
		DWORD GravityScale = unknown;
	}Entity;

	struct
	{
		DWORD MovementServices = unknown;		// CPlayer_MovementServices*
		DWORD WeaponServices = unknown;			// CPlayer_WeaponServices*
		DWORD BulletServices = unknown;			// CCSPlayer_BulletServices*
		DWORD CameraServices = unknown;			// CPlayer_CameraServices*
		DWORD ViewModelServices = unknown;		// CPlayer_ViewModelServices*
		DWORD pClippingWeapon = unknown;			// C_CSWeaponBase*

		DWORD ViewModel = unknown;				// CCSPlayer_ViewModelServices::m_hViewModel
		DWORD StartAccount = unknown;
		DWORD isScoped = unknown;
		DWORD TotalHit = unknown;
		DWORD Pos = unknown;						// C_BasePlayerPawn::m_vOldOrigin
		DWORD MaxHealth = unknown;				// C_BaseEntity::m_iMaxHealth
		DWORD CurrentHealth = unknown;			// C_BaseEntity::m_iHealth
		DWORD GameSceneNode = unknown;			// C_BaseEntity::m_pGameSceneNode
		DWORD BoneArray = unknown;				// CSkeletonInstance_::m_modelState + CGameSceneNode_::m_vecOrigin
		DWORD angEyeAngles = unknown;
		DWORD vecLastClipCameraPos = unknown;
		DWORD iShotsFired = unknown;
		DWORD flFlashMaxAlpha = unknown;
		DWORD flFlashDuration = unknown;
		DWORD aimPunchAngle = unknown;			// C_CSPlayerPawn::m_aimPunchAngle
		DWORD aimPunchCache = unknown;
		DWORD iIDEntIndex = unknown;
		DWORD iTeamNum = unknown;
		DWORD DesiredFov = unknown;
		DWORD iFovStart = unknown;
		DWORD fFlags = unknown;
		DWORD bSpottedByMask = unknown;	// C_CSPlayerPawnBase::entitySpottedState + EntitySpottedState_t::bSpottedByMask
		DWORD AbsVelocity = unknown;
	} Pawn;

	struct
	{
		DWORD RealTime = unknown;
		DWORD FrameCount = unknown;
		DWORD MaxClients = unknown;
		DWORD IntervalPerTick = unknown;
		DWORD CurrentTime = unknown;
		DWORD CurrentTime2 = unknown;
		DWORD TickCount = unknown;
		DWORD IntervalPerTick2 = unknown;
		DWORD CurrentNetchan = unknown;
		DWORD CurrentMap = unknown;
		DWORD CurrentMapName = unknown;
	} GlobalVar;

	struct
	{
		DWORD m_hPawn = unknown;
		DWORD m_pObserverServices = unknown;
		DWORD m_hObserverTarget = unknown;
		DWORD m_hController = unknown;
		DWORD PawnArmor = unknown;
		DWORD HasDefuser = unknown;
		DWORD HasHelmet = unknown;
	} PlayerController;

	struct
	{
		DWORD AttributeManager = unknown;		// C_AttributeContainer
		DWORD FallbackPaintKit = unknown;
		DWORD FallbackSeed = unknown;
		DWORD FallbackWear = unknown;
		DWORD FallbackStatTrak = unknown;
		DWORD szCustomName = unknown;

		DWORD EntityQuality = unknown;			// EconItemView::m_iEntityQuality
		DWORD ItemIDHigh = unknown;				// EconItemView::m_iItemIDHigh
	} EconEntity;

	struct
	{
		DWORD ClippingWeapon = unknown;			// WeaponBase
		DWORD WeaponDataPTR = unknown;
		DWORD szName = unknown;
		DWORD Clip1 = unknown;					// C_BasePlayerWeapon::m_iClip1
		DWORD MaxClip = unknown;					// CBasePlayerWeaponVData::m_iMaxClip1
		DWORD CycleTime = unknown;
		DWORD Penetration = unknown;
		DWORD WeaponType = unknown;
		DWORD Inaccuracy = unknown;				// CCSWeaponBaseVData::m_flInaccuracyMove
		DWORD inReload = unknown;

		DWORD WeaponSize = unknown;
		DWORD ActiveWeapon = unknown;
		DWORD Item = unknown;						// C_AttributeContainer::m_Item
		DWORD ItemDefinitionIndex = unknown;
		DWORD m_MeshGroupMask = unknown;			// CModelState::m_MeshGroupMask
	} WeaponBaseData;

	struct
	{
		DWORD m_bBeingDefused = unknown;
		DWORD m_flDefuseCountDown = unknown;
		DWORD m_nBombSite = unknown;
	} C4;

	struct
	{
		DWORD MoneyServices = unknown;
		DWORD Account = unknown;
		DWORD TotalCashSpent = unknown;
		DWORD CashSpentThisRound = unknown;
	} InGameMoneyServices;

	struct // C_BaseCSGrenadeProjectile
	{ 
		DWORD nSmokeEffectTickBegin = unknown; // int32_t
		DWORD bDidSmokeEffect = unknown; // bool
		DWORD nRandomSeed = 1; // int32_t
		DWORD vSmokeColor = unknown; // Vector
		DWORD vSmokeDetonationPos = unknown; // Vector
		DWORD VoxelFrameData = unknown; // CUtlVector<uint8_t>
		DWORD bSmokeVolumeDataReceived = unknown; // bool
		uintptr_t bSmokeEffectSpawned = unknown; // bool
	} SmokeGrenadeProjectile; 

	namespace Signatures
	{
		const std::string ForceForward = unknown;
		const std::string ForceLeft = unknown;
		const std::string ForceRight = unknown;
		const std::string ForceJump = unknown;
		const std::string ForceCrouch = unknown;

		const std::string InventoryServices = unknown;
		const std::string GlobalVars = unknown;
		const std::string EntityList = unknown;
		const std::string LocalPlayerController = unknown;
		const std::string ViewAngles = unknown;
		const std::string ViewMatrix = unknown;
		const std::string LocalPlayerPawn = unknown;
		const std::string PlantedC4 =unknown;
		const std::string InputSystem = unknown;
		const std::string dwSensitivity = unknown;
	}

	bool UpdateOffsets();
}

```

# 测试

仅供学习使用

```CPP
std::cout<<num <<".name:"<<Entity.Controller.PlayerName\
<<" x:" << Entity.Pawn.Pos.x << "  y:" << Entity.Pawn.Pos.y <<"  z:" << Entity.Pawn.Pos.z \
<<" weapon:"<< Entity.Pawn.WeaponName <<"  health:"<< Entity.Pawn.Health << std::endl;
```

乱码是因为中文没做字符处理

这是一场官匹排位，如果做成图形化界面配上雷达图那可不就是标准的透视cheat了吗

而且对代码的分析发现不仅能获取数据，还能拿到本地实体的控制器，也就是说自瞄也不再话下，听到这里心动的朋友可以去读读刑法:)

![image-20240310034929238](https://s2.loli.net/2024/03/10/LoBFUtHJlya612d.png)

测试发现自身阵亡后将无法再获取数据，这个问题留给大家，答案与上述的原理有关，感兴趣的可以思考一下。