macroScript Pivot_top_bottom
			category: "Ruthenium Converter"
			tooltip: "Ruthenium Converter"
			buttontext: "RuConv"

(

outputFileName = getSaveFileName caption:"Scene Exporter" types:"Scene Files(*.scene)|*.scene" initialDir:(getDir #export) historyCategory:"ExportSceneFiles"

if( outputFileName == undefined ) then (
	return 0
)

global Z2YRotation = rotateXMatrix -90
global file = fopen outputFileName "wb"

fn FindSkinModifier obj = (
	local skinModifier = undefined	
	for i = 1 to obj.modifiers.count do	(
		if obj.modifiers[i].Name == "Skin" then (
			skinModifier = obj.modifiers[i]	
			max modify mode   
			modPanel.setCurrentObject skinModifier	
			exit
		)		
	)
	return skinModifier
)


fn WriteVector3 vec3 = (
	-- write vertex according to X - right, Y - up, Z - forward
	WriteFloat file ( vec3.x)
	WriteFloat file ( vec3.z)
	WriteFloat file (-vec3.y)
)

fn WriteQuaternion quaternion = (
	quaternion = quaternion * Z2YRotation	
	WriteFloat file quaternion.x
	WriteFloat file quaternion.y
	WriteFloat file quaternion.z
	WriteFloat file quaternion.w
)

fn WriteFace face = (
	WriteLong file (face.x - 1)
	WriteLong file (face.y - 1)
	WriteLong file (face.z - 1)
)

fn WriteTextureCoordinates texCoord = (
	WriteFloat file texCoord.x
	WriteFloat file (1 - texCoord.y)
)

fn GetParentScale obj =  (	
	if( obj.parent != undefined ) then (
		return ( in coordsys parent obj.parent.scale ) * GetParentScale( obj.parent )
	) else (
		return [ 1, 1, 1 ]
	)	
)

fn WriteNodePositionRotation obj = (
	-- write position        
	WriteVector3 ( obj.position * GetParentScale ( obj )) 
	-- write rotation
	WriteQuaternion ( obj.rotation )
)

struct EngineMaterial ( diffuseTextureName, opacity )

fn GetMaterialsArray obj = (
	-- collect textures
	local materials = #()
	local currentMaterial = obj.material    

	if( currentMaterial != undefined ) then (
		subMaterials = getNumSubmtls currentMaterial  
		if (subMaterials == 0) then	(
			if( currentMaterial.diffuseMap != undefined ) then (
				append materials ( EngineMaterial diffuseTextureName:( FileNameFromPath currentMaterial.diffuseMap.filename ) opacity:currentMaterial.opacity )
			)
		) else (
			for i = 1 to subMaterials do (
				local subMaterial = getSubmtl currentMaterial i			
				if( subMaterial.diffuseMap != undefined ) then (
					append materials ( EngineMaterial diffuseTextureName:( FileNameFromPath subMaterial.diffuseMap.filename ) opacity:subMaterial.opacity )
				)
			)
		)
	)	
	return materials
)

fn GetBoneID boneName meshes = (
	local id = 0	
	for obj in meshes do (
		if obj.name == bonename then (
			return id
		)		
		id = id + 1
	)	
	return id
)

fn IsMesh fObject = (
	return SuperClassOf fObject == GeometryClass;
)

global animStart = animationRange.start.frame as integer 
global animEnd = animationRange.end.frame as integer 
global framesCount = (animEnd - animStart + 1)	

if outputFileName != undefined then (
	animate off
	
	sliderTime = 0

	-- grab all objects from scene into their groups
	local lightList = #()
	local meshList = #()	

	for currentObject in objects do	( 
		if SuperClassOf currentObject == GeometryClass or SuperClassOf currentObject == Helper then (
			append meshList currentObject			
		)		
		if SuperClassOf currentObject == Light then (
			append lightList currentObject
		)
	)
	
	WriteLong file ( framesCount )
	WriteLong file ( objects.count )
	WriteLong file ( meshList.count )
	WriteLong file ( lightList.count )
	
	-- write meshes data
	for currentObject in meshList do (   
		local frozen = currentObject.isFrozen
		
		currentObject.isFrozen = false
		
		local hasAnimation = 0
		
		if currentObject.position.isAnimated == true then (
			hasAnimation = 1
		)		
		if currentObject.rotation.isAnimated == true then (
			hasAnimation = 1
		)
			
		local skinModifier = FindSkinModifier currentObject  			
		local userProp = getUserPropBuffer currentObject   
		
		local mesh = undefined	
		if IsMesh(currentObject) then (
			mesh = in coordsys world at time 0 snapShotAsMesh ( currentObject )
		)
		
		-- properties buffer
		writeString file userProp    

		-- geometry parameters
		if IsMesh(currentObject) then (
			writeLong file mesh.numverts     
			writeLong file mesh.numtverts
			writeLong file mesh.numfaces
		) else ( 
			writeLong file 0    
			writeLong file 0
			writeLong file 0			
		)
		
		-- animation 
		if( skinModifier != undefined ) then (
			writeLong file 1
		) else (
			writeLong file 0
		)
		
		writeLong file hasAnimation
	   
		-- write name
		writeString file currentObject.name
		
		in coordsys parent (  
			if hasAnimation == 1 then (
				-- write keyframes
				for frame = animStart to animEnd do (
					at time frame WriteNodePositionRotation currentObject	
				)
			)else ( 
				-- write only position and rotation	
				at time 0 WriteNodePositionRotation currentObject
			)
		)
		
		-- collect textures
		local materials = GetMaterialsArray currentObject
		
		-- write texture file names
		WriteLong file materials.count
		
		for engMat in materials do (
			WriteString file engMat.diffuseTextureName
			WriteFloat file engMat.opacity
		)

		-- write vertex coordinates at start frame
		if IsMesh(currentObject) then (
			local invTrans = inverse currentObject.transform
			in coordsys world (
				at time 0 (
					for i = 1 to mesh.numverts do (
						WriteVector3 ((getVert mesh i) * invTrans * currentObject.scale )   
					)
				)
			)
		)
		
		-- export bones
		if IsMesh(currentObject) then (
			if( skinModifier != undefined ) then (
				-- write bone weights
				for i = 1 to mesh.numVerts do (
					local weightCount = skinops.GetVertexWeightCount skinModifier i
					
					if( weightCount > 4 ) then (
						weightCount = 4
					)
					WriteLong file weightCount
					
					for k = 1 to weightCount do (
						local weight = skinops.GetVertexWeight skinModifier i k
						local boneID =GetBoneID ( skinops.GetBoneName skinModifier ( skinops.GetVertexWeightBoneID skinModifier i k ) 1 ) meshList
						
						WriteLong file boneID
						WriteFloat file weight
					)
				)
			)
		)
		
		-- write texture coordinates	
		if IsMesh(currentObject) then (
			WriteLong file (meshop.GetNumMaps mesh) -- not needed, leaved for compatibility
		) else (
			WriteLong file 0 -- not needed, leaved for compatibility
		)
		WriteLong file 1
		
		if IsMesh(currentObject) then (
			if( mesh.numtverts > 0 ) then (
				local numMapVerts = meshop.GetNumMapVerts mesh 1			
				for v = 1 to numMapVerts do (
					WriteTextureCoordinates ( meshop.GetMapVert mesh 1 v )
				)
			)
		)
		
		-- write faces
		if IsMesh(currentObject) then (
			local normalInvTrans = inverse (currentObject.transform.rotation as matrix3)
			for f = 1 to mesh.numfaces do (
				local face = getFace mesh f
			  
				WriteFace face
				
				if ( mesh.numtverts > 0 ) then (
					WriteFace ( getTVFace mesh f )
				) else (
					WriteFace face    
				)
				
				if (materials.count > 1) then (
					writeLong file ((getFaceMatID mesh f) - 1)
				) else (
					writeLong file 0    
				)
				
				local normals = meshop.getFaceRNormals mesh f
				
				WriteVector3 (normals[1] * normalInvTrans)
				WriteVector3 (normals[2] * normalInvTrans)
				WriteVector3 (normals[3] * normalInvTrans)
			)
		)
		
		if(mesh != undefined) then (
			delete mesh  	  
		)
		
		currentObject.isFrozen = frozen
	)

	-- write lights
	for currentLight in lightList do (   
		WriteString file currentLight.name
			
		if currentLight.type == #omni then (
			WriteLong file 0
		)
		if currentLight.type == #freeSpot then (
			WriteLong file 1
		)
		
		-- write basic parameters
		
		-- write color
		local diffuseColor = currentLight.rgb
		
		WriteLong file ( diffuseColor.r )
		WriteLong file ( diffuseColor.g )
		WriteLong file ( diffuseColor.b )
		
		-- write attenuation
		WriteFloat file ( currentLight.DecayRadius * currentLight.scale.x )
		
		WriteFloat file (currentLight.multiplier )
		-- write position
		in coordsys parent at time 0 WriteVector3(currentLight.position * GetParentScale ( currentLight ))
		
		-- write type-specific parameters
		if currentLight.type == #freeSpot then (			
			WriteFloat file currentLight.Hotspot -- inner cone angle 
			WriteFloat file currentLight.Falloff -- outer cone angle
			
			in coordsys parent WriteQuaternion currentLight.rotation -- write light direction
		)	
	)
	
	-- write hierarchy
	for currentObject in objects do (
		local parentName = "World";

		if( currentObject.parent != undefined ) then
			parentName = currentObject.parent.name

		writeString file currentObject.name
		writeString file parentName    
	)

	fclose file
	
	ShellLaunch "C:\Mine\External\converter.exe" outputFileName
	
	undo on
	EnableSceneRedraw	
)

)
