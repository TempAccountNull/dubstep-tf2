#pragma once
#include "CViewRender.hpp"

struct player_info_t
{ //0x158
	unsigned char	szName[32];
	int				userID;
	char			guid[32 + 1];
	uint32_t		friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	bool			isreplay;
	uint32_t		customFiles[4];
	unsigned char   pad[0x999];
};

struct mstudiobbox_t
{
	int					bone;
	int					group;				// intersection group
	vec3_t				bbmin;				// bounding box
	vec3_t				bbmax;	
	int					szhitboxnameindex;	// offset to the name of the hitbox.
	int					unused[8];

	const char* pszHitboxName()
	{
		if( szhitboxnameindex == 0 )
			return "";

		return ((const char*)this) + szhitboxnameindex + 1;
	}

	mstudiobbox_t() {}

private:
	// No copy constructors allowed
	mstudiobbox_t(const mstudiobbox_t& vOther);
};

struct mstudiohitboxset_t
{
	
	int					sznameindex;
	inline char * const	pszName( void ) const { return ((char *)this) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t *pHitbox( int i ) const { return (mstudiobbox_t *)(((byte *)this) + hitboxindex) + i; };
};

// bones
struct mstudiobone_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	unsigned int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	vec3_t				pos;
	float				quat[4];
	vec3_t			rot;
	// compression scale
	vec3_t				posscale;
	vec3_t				rotscale;

	matrix3x4_t			poseToBone;
	float				qAlignment[4];
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void *pProcedure( ) const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropidx; }
	int					contents;		// See BSPFlags.h for the contents flags

	int					unused[8];		// remove as appropriate

	mstudiobone_t(){}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};

struct studiohdr_t
{
	int					id;
	int					version;

	int					checksum;		// this has to be the same in the phy and vtx files to load!
	int unused_2;
	//inline const char *	pszName( void ) const { if (studiohdr2index && pStudioHdr2()->pszName()) return pStudioHdr2()->pszName(); else return name; }
	char				name[64];
	int					length;


	vec3_t				eyeposition;	// ideal eye position

	vec3_t				illumposition;	// illumination center
	
	vec3_t				hull_min;		// ideal movement hull size
	vec3_t				hull_max;			

	vec3_t				view_bbmin;		// clipping bounding box
	vec3_t				view_bbmax;		

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	inline mstudiobone_t *pBone( int i ) const { return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
	int					RemapSeqBone( int iSequence, int iLocalBone ) const;	// maps local sequence bone to global bone
	int					RemapAnimBone( int iAnim, int iLocalBone ) const;		// maps local animations bone to global bone

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	//inline mstudiobonecontroller_t *pBonecontroller( int i ) const { Assert( i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

	int					numhitboxsets;
	int					hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t	*pHitboxSet( int i ) const 
	{ 
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex ) + i; 
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t *pHitbox( int i, int set ) const 
	{ 
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return NULL;

		return s->pHitbox( i );
	};

	// Calls through to set to get hitbox count for set
	inline int			iHitboxCount( int set ) const
	{
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
  	//inline mstudioanimdesc_t *pLocalAnimdesc( int i ) const { if (i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t *)(((byte *)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
  	//inline mstudioseqdesc_t *pLocalSeqdesc( int i ) const { if (i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + localseqindex) + i; };

//public:
	bool				SequencesAvailable() const;
	int					GetNumSeq() const;
	//mstudioanimdesc_t	&pAnimdesc( int i ) const;
	//mstudioseqdesc_t	&pSeqdesc( int i ) const;
	int					iRelativeAnim( int baseseq, int relanim ) const;	// maps seq local anim reference to global anim index
	int					iRelativeSeq( int baseseq, int relseq ) const;		// maps seq local seq reference to global seq index

//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
//public:
	int					GetSequenceActivity( int iSequence );
	void				SetSequenceActivity( int iSequence, int iActivity );
	int					GetActivityListVersion( void );
	void				SetActivityListVersion( int version ) const;
	int					GetEventListVersion( void );
	void				SetEventListVersion( int version );
	
	// raw textures
	int					numtextures;
	int					textureindex;
	//inline mstudiotexture_t *pTexture( int i ) const { Assert( i >= 0 && i < numtextures ); return (mstudiotexture_t *)(((byte *)this) + textureindex) + i; }; 


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char			*pCdtexture( int i ) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short		*pSkinref( int i ) const { return (short *)(((byte *)this) + skinindex) + i; };

	int					numbodyparts;		
	int					bodypartindex;
	//inline mstudiobodyparts_t	*pBodypart( int i ) const { return (mstudiobodyparts_t *)(((byte *)this) + bodypartindex) + i; };

	// queryable attachable points
//private:
	int					numlocalattachments;
	int					localattachmentindex;
	//inline mstudioattachment_t	*pLocalAttachment( int i ) const { Assert( i >= 0 && i < numlocalattachments); return (mstudioattachment_t *)(((byte *)this) + localattachmentindex) + i; };
//public:
	int					GetNumAttachments( void ) const;
	//const mstudioattachment_t &pAttachment( int i ) const;
	int					GetAttachmentBone( int i );
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone( int iAttachment, int iBone );

	// animation node to animation node transition graph
//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	//inline char			*pszLocalNodeName( int iNode ) const { Assert( iNode >= 0 && iNode < numlocalnodes); return (((char *)this) + *((int *)(((byte *)this) + localnodenameindex) + iNode)); }
	//inline byte			*pLocalTransition( int i ) const { Assert( i >= 0 && i < (numlocalnodes * numlocalnodes)); return (byte *)(((byte *)this) + localnodeindex) + i; };

//public:
	int					EntryNode( int iSequence );
	int					ExitNode( int iSequence );
	char				*pszNodeName( int iNode );
	int					GetTransition( int iFrom, int iTo ) const;

	int					numflexdesc;
	int					flexdescindex;
	//inline mstudioflexdesc_t *pFlexdesc( int i ) const { Assert( i >= 0 && i < numflexdesc); return (mstudioflexdesc_t *)(((byte *)this) + flexdescindex) + i; };

	int					numflexcontrollers;
	int					flexcontrollerindex;
	//inline mstudioflexcontroller_t *pFlexcontroller( LocalFlexController_t i ) const { Assert( numflexcontrollers == 0 || ( i >= 0 && i < numflexcontrollers ) ); return (mstudioflexcontroller_t *)(((byte *)this) + flexcontrollerindex) + i; };

	int					numflexrules;
	int					flexruleindex;
	//inline mstudioflexrule_t *pFlexRule( int i ) const { Assert( i >= 0 && i < numflexrules); return (mstudioflexrule_t *)(((byte *)this) + flexruleindex) + i; };

	int					numikchains;
	int					ikchainindex;
	//inline mstudioikchain_t *pIKChain( int i ) const { Assert( i >= 0 && i < numikchains); return (mstudioikchain_t *)(((byte *)this) + ikchainindex) + i; };

	int					nummouths;
	int					mouthindex;
	//inline mstudiomouth_t *pMouth( int i ) const { Assert( i >= 0 && i < nummouths); return (mstudiomouth_t *)(((byte *)this) + mouthindex) + i; };

//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	//inline mstudioposeparamdesc_t *pLocalPoseParameter( int i ) const { Assert( i >= 0 && i < numlocalposeparameters); return (mstudioposeparamdesc_t *)(((byte *)this) + localposeparamindex) + i; };
//public:
	int					GetNumPoseParameters( void ) const;
	//const mstudioposeparamdesc_t &pPoseParameter( int i );
	int					GetSharedPoseParameter( int iSequence, int iLocalPose ) const;

	int					surfacepropindex;
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropindex; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	//inline mstudioiklock_t *pLocalIKAutoplayLock( int i ) const { Assert( i >= 0 && i < numlocalikautoplaylocks); return (mstudioiklock_t *)(((byte *)this) + localikautoplaylockindex) + i; };

	int					GetNumIKAutoplayLocks( void ) const;
	//const mstudioiklock_t &pIKAutoplayLock( int i );
	int					CountAutoplaySequences() const;
	int					CopyAutoplaySequences( unsigned short *pOut, int outCount ) const;
	int					GetAutoplayList( unsigned short **pOut ) const;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	//inline mstudiomodelgroup_t *pModelGroup( int i ) const { Assert( i >= 0 && i < numincludemodels); return (mstudiomodelgroup_t *)(((byte *)this) + includemodelindex) + i; };
	// implementation specific call to get a named model
	const studiohdr_t	*FindModel( void **cache, char const *modelname ) const;

	// implementation specific back pointer to virtual data
	mutable void		*virtualModel;
	//virtualmodel_t		*GetVirtualModel( void ) const;

	// for demand loaded animation blocks
	int					szanimblocknameindex;	
	//inline char * const pszAnimBlockName( void ) const { return ((char *)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	//inline mstudioanimblock_t *pAnimBlock( int i ) const { Assert( i > 0 && i < numanimblocks); return (mstudioanimblock_t *)(((byte *)this) + animblockindex) + i; };
	mutable void		*animblockModel;
	//byte *				GetAnimBlock( int i ) const;

	int					bonetablebynameindex;
	//inline const byte	*GetBoneTableSortedByName() const { return (byte *)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void				*pVertexBase;
	void				*pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;
	
	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;

	byte				unused[1];

	int					unused4; // zero out if version < 47

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;
	//mstudioflexcontrollerui_t *pFlexControllerUI( int i ) const { Assert( i >= 0 && i < numflexcontrollerui); return (mstudioflexcontrollerui_t *)(((byte *)this) + flexcontrolleruiindex) + i; }

	float				flVertAnimFixedPointScale;
	//inline float		VertAnimFixedPointScale() const { return ( flags & STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE ) ? flVertAnimFixedPointScale : 1.0f / 4096.0f; }

	int					unused3[1];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int					studiohdr2index;
	//studiohdr2_t*		pStudioHdr2() const { return (studiohdr2_t *)( ( (byte *)this ) + studiohdr2index ); }

	// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
	//int					NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->numsrcbonetransform : 0; }
	//const mstudiosrcbonetransform_t* SrcBoneTransform( int i ) const { Assert( i >= 0 && i < NumSrcBoneTransforms()); return (mstudiosrcbonetransform_t *)(((byte *)this) + pStudioHdr2()->srcbonetransformindex) + i; }

	//inline int			IllumPositionAttachmentIndex() const { return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0; }

	//inline float		MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; } // default to cos(30) if not set

	//inline mstudiolinearbone_t *pLinearBones() const { return studiohdr2index ? pStudioHdr2()->pLinearBones() : NULL; }

	//inline int			BoneFlexDriverCount() const { return studiohdr2index ? pStudioHdr2()->m_nBoneFlexDriverCount : 0; }
	//inline const mstudioboneflexdriver_t* BoneFlexDriver( int i ) const { Assert( i >= 0 && i < BoneFlexDriverCount() ); return studiohdr2index ? pStudioHdr2()->pBoneFlexDriver( i ) : NULL; }

	// NOTE: No room to add stuff? Up the .mdl file format version 
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int					unused2[1];

	studiohdr_t() {}

private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};

struct model_t
{
	int pad;
	char szName[260];
};

class IEngineClient
{
public:
	//virtual int GetIntersectingSurfaces( const class model_t* model, const vec3_t& vCenter, const float radius, const bool bOnlyVisibleSurfaces, class SurfInfo* pInfos, const int nMaxInfos ) = 0;
	//virtual vec3_t GetLightForPoint( const vec3_t& pos, bool bClamp ) = 0;
	//virtual class IMaterial* TraceLineMaterialAndLighting( const vec3_t& start, const vec3_t& end, vec3_t& diffuseLightColor, vec3_t& baseColor ) = 0;
	//virtual const char* ParseFile( const char* data, char* token, int maxlen ) = 0;
	//virtual bool CopyFile( const char* source, const char* destination ) = 0;
	//virtual void GetScreenSize( int& width, int& height ) = 0;
	//virtual void ServerCmd( const char* szCmdString, bool bReliable = true ) = 0;
	//virtual void ClientCmd( const char* szCmdString ) = 0;

	bool GetPlayerInfo( int ent_num, player_info_t* pinfo )
	{
		typedef bool(__thiscall* OriginalFn)( void*, int, player_info_t*);
		return GetVFunc< OriginalFn >( this, 28 )( this, ent_num, pinfo );
	}
	//virtual int GetPlayerForUserID( int userID ) = 0;
	//virtual client_textmessage_t* TextMessageGet( const char* pName ) = 0; // 10
	//virtual bool Con_IsVisible( void ) = 0;
	//virtual const model_t* LoadModel( const char* pName, bool bProp = false ) = 0;
	//virtual float GetLastTimeStamp( void ) = 0;
	//virtual CSentence* GetSentence( CAudioSource* pAudioSource ) = 0; // 15
	//virtual float GetSentenceLength( CAudioSource* pAudioSource ) = 0;
	//virtual bool IsStreaming( CAudioSource* pAudioSource ) const = 0;
	//virtual void GetViewAngles( QAngle& va ) = 0;
	//virtual void SetViewAngles( QAngle& va ) = 0;
	//virtual int GetMaxClients( void ) = 0; // 20
	//virtual const char* Key_LookupBinding( const char* pBinding ) = 0;
	//virtual const char* Key_BindingForKey( int& code ) = 0;
	//virtual void Key_SetBinding( int, char const* ) = 0;
	//virtual void StartKeyTrapMode( void ) = 0;
	//virtual bool CheckDoneKeyTrapping( int& code ) = 0;
	//virtual bool IsInGame( void ) = 0;
	//virtual bool IsConnected( void ) = 0;
	//virtual bool IsDrawingLoadingImage( void ) = 0;
	//virtual void HideLoadingPlaque( void ) = 0;
	//virtual void Con_NPrintf( int pos, const char* fmt, ... ) = 0; // 30
	//virtual void Con_NXPrintf( const struct con_nprint_s* info, const char* fmt, ... ) = 0;
	//virtual int IsBoxVisible( const Vector& mins, const Vector& maxs ) = 0;
	//virtual int IsBoxInViewCluster( const Vector& mins, const Vector& maxs ) = 0;
	//virtual bool CullBox( const Vector& mins, const Vector& maxs ) = 0;
	//virtual void Sound_ExtraUpdate( void ) = 0;
	//virtual const char* GetGameDirectory( void ) = 0;
	//virtual const VMatrix& WorldToScreenMatrix() = 0;
	//virtual const VMatrix& WorldToViewMatrix() = 0;
	//virtual int GameLumpVersion( int lumpId ) const = 0;
	//virtual int GameLumpSize( int lumpId ) const = 0; // 40
	//virtual bool LoadGameLump( int lumpId, void* pBuffer, int size ) = 0;
	//virtual int LevelLeafCount() const = 0;
	//virtual ISpatialQuery* GetBSPTreeQuery() = 0;
	//virtual void LinearToGamma( float* linear, float* gamma ) = 0;
	//virtual float LightStyleValue( int style ) = 0; // 45
	//virtual void ComputeDynamicLighting( const Vector& pt, const Vector* pNormal, Vector& color ) = 0;
	//virtual void GetAmbientLightColor( Vector& color ) = 0;
	//virtual int GetDXSupportLevel() = 0;
	//virtual bool SupportsHDR() = 0;
	//virtual void Mat_Stub( IMaterialSystem* pMatSys ) = 0; // 50
	//virtual void GetChapterName( char* pchBuff, int iMaxLength ) = 0;
	//virtual char const* GetLevelName( void ) = 0;
	//virtual char const* GetLevelNameShort( void ) = 0;
	//virtual char const* GetMapGroupName( void ) = 0;
	//virtual struct IVoiceTweak_s* GetVoiceTweakAPI( void ) = 0;
	//virtual void SetVoiceCasterID( unsigned int someint ) = 0; // 56
	//virtual void EngineStats_BeginFrame( void ) = 0;
	//virtual void EngineStats_EndFrame( void ) = 0;
	//virtual void FireEvents() = 0;
	//virtual int GetLeavesArea( unsigned short* pLeaves, int nLeaves ) = 0;
	//virtual bool DoesBoxTouchAreaFrustum( const Vector& mins, const Vector& maxs, int iArea ) = 0; // 60
	//virtual int GetFrustumList( Frustum_t** pList, int listMax ) = 0;
	//virtual bool ShouldUseAreaFrustum( int i ) = 0;
	//virtual void SetAudioState( const AudioState_t& state ) = 0;
	//virtual int SentenceGroupPick( int groupIndex, char* name, int nameBufLen ) = 0;
	//virtual int SentenceGroupPickSequential( int groupIndex, char* name, int nameBufLen, int sentenceIndex, int reset ) = 0;
	//virtual int SentenceIndexFromName( const char* pSentenceName ) = 0;
	//virtual const char* SentenceNameFromIndex( int sentenceIndex ) = 0;
	//virtual int SentenceGroupIndexFromName( const char* pGroupName ) = 0;
	//virtual const char* SentenceGroupNameFromIndex( int groupIndex ) = 0;
	//virtual float SentenceLength( int sentenceIndex ) = 0;
	//virtual void ComputeLighting( const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector* pBoxColors = NULL ) = 0;
	//virtual void ActivateOccluder( int nOccluderIndex, bool bActive ) = 0;
	//virtual bool IsOccluded( const Vector& vecAbsMins, const Vector& vecAbsMaxs ) = 0; // 74
	//virtual int GetOcclusionViewId( void ) = 0;
	//virtual void* SaveAllocMemory( size_t num, size_t size ) = 0;
	//virtual void SaveFreeMemory( void* pSaveMem ) = 0;
	//virtual INetChannelInfo* GetNetChannelInfo( void ) = 0;
	//virtual void DebugDrawPhysCollide( const CPhysCollide* pCollide, IMaterial* pMaterial, const matrix3x4_t& transform, const Color& color ) = 0; //79
	//virtual void CheckPoint( const char* pName ) = 0; // 80
	//virtual void DrawPortals() = 0;
	//virtual bool IsPlayingDemo( void ) = 0;
	//virtual bool IsRecordingDemo( void ) = 0;
	//virtual bool IsPlayingTimeDemo( void ) = 0;
	//virtual int GetDemoRecordingTick( void ) = 0;
	//virtual int GetDemoPlaybackTick( void ) = 0;
	//virtual int GetDemoPlaybackStartTick( void ) = 0;
	//virtual float GetDemoPlaybackTimeScale( void ) = 0;
	//virtual int GetDemoPlaybackTotalTicks( void ) = 0;
	//virtual bool IsPaused( void ) = 0; // 90
	//virtual float GetTimescale( void ) const = 0;
	//virtual bool IsTakingScreenshot( void ) = 0;
	//virtual bool IsHLTV( void ) = 0;
	//virtual bool IsLevelMainMenuBackground( void ) = 0;
	//virtual void GetMainMenuBackgroundName( char* dest, int destlen ) = 0;
	//virtual void SetOcclusionParameters( const int /*OcclusionParams_t*/ & params ) = 0; // 96
	//virtual void GetUILanguage( char* dest, int destlen ) = 0;
	//virtual int IsSkyboxVisibleFromPoint( const vec3_t& vecPoint ) = 0;
	//virtual const char* GetMapEntitiesString() = 0;
	//virtual bool IsInEditMode( void ) = 0; // 100
	//virtual float GetScreenAspectRatio( int viewportWidth, int viewportHeight ) = 0;
	//virtual bool REMOVED_SteamRefreshLogin( const char* password, bool isSecure ) = 0; // 100
	//virtual bool REMOVED_SteamProcessCall( bool& finished ) = 0;
	//virtual unsigned int GetEngineBuildNumber() = 0; // engines build
	//virtual const char* GetProductVersionString() = 0; // mods version number (steam.inf)
	//virtual void GrabPreColorCorrectedFrame( int x, int y, int width, int height ) = 0;
	//virtual bool IsHammerRunning() const = 0;
	//virtual void ExecuteClientCmd( const char* szCmdString ) = 0; //108
	//virtual bool MapHasHDRLighting( void ) = 0;
	//virtual bool MapHasLightMapAlphaData( void ) = 0;
	//virtual int GetAppID() = 0;
	//virtual vec3_t GetLightForPointFast( const vec3_t& pos, bool bClamp ) = 0;
	//virtual void ClientCmd_Unrestricted1( char const*, int, bool );
	//virtual void ClientCmd_Unrestricted( const char* szCmdString, const char* newFlag ) = 0; // 114, new flag, quick testing shows setting 0 seems to work, haven't looked into it.
	////Forgot to add this line, but make sure to format all unrestricted calls now with an extra , 0
	////Ex:
	////	I::Engine->ClientCmd_Unrestricted( charenc( "cl_mouseenable 1" ) , 0);
	////	I::Engine->ClientCmd_Unrestricted( charenc( "crosshair 1" ) , 0);
	//virtual void SetRestrictServerCommands( bool bRestrict ) = 0;
	//virtual void SetRestrictClientCommands( bool bRestrict ) = 0;
	//virtual void SetOverlayBindProxy( int iOverlayID, void* pBindProxy ) = 0;
	//virtual bool CopyFrameBufferToMaterial( const char* pMaterialName ) = 0;
	//virtual void ReadConfiguration( const int iController, const bool readDefault ) = 0;
	//virtual void SetAchievementMgr( IAchievementMgr* pAchievementMgr ) = 0;
	//virtual IAchievementMgr* GetAchievementMgr() = 0;
	//virtual bool MapLoadFailed( void ) = 0;
	//virtual void SetMapLoadFailed( bool bState ) = 0;
	//virtual bool IsLowViolence() = 0;
	//virtual const char* GetMostRecentSaveGame( void ) = 0;
	//virtual void SetMostRecentSaveGame( const char* lpszFilename ) = 0;
	//virtual void StartXboxExitingProcess() = 0;
	//virtual bool IsSaveInProgress() = 0;
	//virtual bool IsAutoSaveDangerousInProgress( void ) = 0;
	//virtual unsigned int OnStorageDeviceAttached( int iController ) = 0;
	//virtual void OnStorageDeviceDetached( int iController ) = 0;
	//virtual char* const GetSaveDirName( void ) = 0;
	//virtual void WriteScreenshot( const char* pFilename ) = 0;
	//virtual void ResetDemoInterpolation( void ) = 0;
	//virtual int GetActiveSplitScreenPlayerSlot() = 0;
	//virtual int SetActiveSplitScreenPlayerSlot( int slot ) = 0;
	//virtual bool SetLocalPlayerIsResolvable( char const* pchContext, int nLine, bool bResolvable ) = 0;
	//virtual bool IsLocalPlayerResolvable() = 0;
	//virtual int GetSplitScreenPlayer( int nSlot ) = 0;
	//virtual bool IsSplitScreenActive() = 0;
	//virtual bool IsValidSplitScreenSlot( int nSlot ) = 0;
	//virtual int FirstValidSplitScreenSlot() = 0; // -1 == invalid
	//virtual int NextValidSplitScreenSlot( int nPreviousSlot ) = 0; // -1 == invalid
	//virtual ISPSharedMemory* GetSinglePlayerSharedMemorySpace( const char* szName, int ent_num = ( 1 << 11 ) ) = 0;
	//virtual void ComputeLightingCube( const Vector& pt, bool bClamp, Vector* pBoxColors ) = 0;
	//virtual void RegisterDemoCustomDataCallback( const char* szCallbackSaveID, pfnDemoCustomDataCallback pCallback ) = 0;
	//virtual void RecordDemoCustomData( pfnDemoCustomDataCallback pCallback, const void* pData, size_t iDataLength ) = 0;
	//virtual void SetPitchScale( float flPitchScale ) = 0;
	//virtual float GetPitchScale( void ) = 0;
	//virtual bool LoadFilmmaker() = 0;
	//virtual void UnloadFilmmaker() = 0;
	//virtual void SetLeafFlag( int nLeafIndex, int nFlagBits ) = 0;
	//virtual void RecalculateBSPLeafFlags( void ) = 0;
	//virtual bool DSPGetCurrentDASRoomNew( void ) = 0;
	//virtual bool DSPGetCurrentDASRoomChanged( void ) = 0;
	//virtual bool DSPGetCurrentDASRoomSkyAbove( void ) = 0;
	//virtual float DSPGetCurrentDASRoomSkyPercent( void ) = 0;
	//virtual void SetMixGroupOfCurrentMixer( const char* szgroupname, const char* szparam, float val, int setMixerType ) = 0;
	//virtual int GetMixLayerIndex( const char* szmixlayername ) = 0;
	//virtual void SetMixLayerLevel( int index, float level ) = 0;
	//virtual int GetMixGroupIndex( char const* groupname ) = 0;
	//virtual void SetMixLayerTriggerFactor( int i1, int i2, float fl ) = 0;
	//virtual void SetMixLayerTriggerFactor( char const* char1, char const* char2, float fl ) = 0;
	//virtual bool IsCreatingReslist() = 0;
	//virtual bool IsCreatingXboxReslist() = 0;
	//virtual void SetTimescale( float flTimescale ) = 0;
	//virtual void SetGamestatsData( CGamestatsData* pGamestatsData ) = 0;
	//virtual CGamestatsData* GetGamestatsData() = 0;
	//virtual void GetMouseDelta( int& dx, int& dy, bool b ) = 0; // unknown
	//virtual const char* Key_LookupBindingEx( const char* pBinding, int iUserId = -1, int iStartCount = 0, int iAllowJoystick = -1 ) = 0;
	//virtual int Key_CodeForBinding( char const*, int, int, int ) = 0;
	//virtual void UpdateDAndELights( void ) = 0;
	//virtual int GetBugSubmissionCount() const = 0;
	//virtual void ClearBugSubmissionCount() = 0;
	//virtual bool DoesLevelContainWater() const = 0;
	//virtual float GetServerSimulationFrameTime() const = 0;
	//virtual void SolidMoved( class IClientEntity* pSolidEnt, class ICollideable* pSolidCollide, const Vector* pPrevAbsOrigin, bool accurateBboxTriggerChecks ) = 0;
	//virtual void TriggerMoved( class IClientEntity* pTriggerEnt, bool accurateBboxTriggerChecks ) = 0;
	//virtual void ComputeLeavesConnected( const Vector& vecOrigin, int nCount, const int* pLeafIndices, bool* pIsConnected ) = 0;
	//virtual bool IsInCommentaryMode( void ) = 0;
	//virtual void SetBlurFade( float amount ) = 0;
	//virtual bool IsTransitioningToLoad() = 0;
	//virtual void SearchPathsChangedAfterInstall() = 0;
	//virtual void ConfigureSystemLevel( int nCPULevel, int nGPULevel ) = 0;
	//virtual void SetConnectionPassword( char const* pchCurrentPW ) = 0;
	//virtual CSteamAPIContext* GetSteamAPIContext() = 0;
	//virtual void SubmitStatRecord( char const* szMapName, unsigned int uiBlobVersion, unsigned int uiBlobSize, const void* pvBlob ) = 0;
	//virtual void ServerCmdKeyValues( KeyValues* pKeyValues ) = 0; // 203
	//virtual void SpherePaintSurface( const model_t* model, const Vector& location, unsigned char chr, float fl1, float fl2 ) = 0;
	//virtual bool HasPaintmap( void ) = 0;
	//virtual void EnablePaintmapRender() = 0;
	////virtual void                TracePaintSurface( const model_t *model, const Vector& position, float radius, CUtlVector<Color>& surfColors ) = 0;
	//virtual void SphereTracePaintSurface( const model_t* model, const Vector& position, const Vector& vec2, float radius, /*CUtlVector<unsigned char, CUtlMemory<unsigned char, int>>*/ int& utilVecShit ) = 0;
	//virtual void RemoveAllPaint() = 0;
	//virtual void PaintAllSurfaces( unsigned char uchr ) = 0;
	//virtual void RemovePaint( const model_t* model ) = 0;
	//virtual bool IsActiveApp() = 0;
	//virtual bool IsClientLocalToActiveServer() = 0;
	//virtual void TickProgressBar() = 0;
	//virtual InputContextHandle_t GetInputContext( int /*EngineInputContextId_t*/ id ) = 0;
	//virtual void GetStartupImage( char* filename, int size ) = 0;
	//virtual bool IsUsingLocalNetworkBackdoor( void ) = 0;
	//virtual void SaveGame( const char*, bool, char*, int, char*, int ) = 0;
	//virtual void GetGenericMemoryStats( /* GenericMemoryStat_t */ void** ) = 0;
	//virtual bool GameHasShutdownAndFlushedMemory( void ) = 0;
	//virtual int GetLastAcknowledgedCommand( void ) = 0;
	//virtual void FinishContainerWrites( int i ) = 0;
	//virtual void FinishAsyncSave( void ) = 0;
	//virtual int GetServerTick( void ) = 0;
	//virtual const char* GetModDirectory( void ) = 0;
	//virtual bool AudioLanguageChanged( void ) = 0;
	//virtual bool IsAutoSaveInProgress( void ) = 0;
	//virtual void StartLoadingScreenForCommand( const char* command ) = 0;
	//virtual void StartLoadingScreenForKeyValues( KeyValues* values ) = 0;
	//virtual void SOSSetOpvarFloat( const char*, float ) = 0;
	//virtual void SOSGetOpvarFloat( const char*, float& ) = 0;
	//virtual bool IsSubscribedMap( const char*, bool ) = 0;
	//virtual bool IsFeaturedMap( const char*, bool ) = 0;
	//virtual void GetDemoPlaybackParameters( void ) = 0;
	//virtual int GetClientVersion( void ) = 0;
	//virtual bool IsDemoSkipping( void ) = 0;
	//virtual void SetDemoImportantEventData( const KeyValues* values ) = 0;
	//virtual void ClearEvents( void ) = 0;
	//virtual int GetSafeZoneXMin( void ) = 0;
	//virtual bool IsVoiceRecording( void ) = 0;
	//virtual void ForceVoiceRecordOn( void ) = 0;
	//virtual bool IsReplay( void ) = 0;
};

inline IEngineClient* EngineClient = nullptr;