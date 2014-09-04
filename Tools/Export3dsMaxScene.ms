utility exportscene "3dsMax Scene Converter" (
	
	edittext folderprefix "Folder:" text:"maxexport"
	
	button exportscn "Export scene" offset:[0,20]
	progressbar scnprogress color:green

	button exportobj "Export objects" offset:[0,20]
	progressbar objprogress color:green
	
	global workingdir = "C:\\Users\\Johan\\Desktop\\Data\\"
	
	function GetFileName fullpath = (
		tokens = filterString fullpath "\\"
		if(tokens.count > 0) then (
			return tokens[tokens.count]
		) else (
			return "";
		)
	)
	
	function WriteSCN = (
		
		-- Paden bijhouden, copy van/naar WriteOBJ
		datafolder = ""
		if(folderprefix.text != "") then ( -- geen loze slash gebruiken
			datafolder = folderprefix.text + "\\"
		)

		scenedir = workingdir + "Scenes\\" + datafolder
		materialdir = workingdir + "Materials\\" + datafolder
		modeldir = workingdir + "Models\\" + datafolder

		iterator = 0
		
		makeDir scenedir
		makeDir materialdir
		
		scenefilename = scenedir + "MaxExport.script"
		scenefile = createFile scenefilename
		
		-- Maak als laatst een lijst met de exports
		for obj in geometry do (
			
			-- Vooruitgang
			iterator = iterator+1
			scnprogress.value = 100.0f*iterator/geometry.count
			
			-- tmp, haal nummer weg...
			internalname = obj.name
			
			-- Kopietje uit WriteOBJ
			modelfilename = internalname + ".obj"
			modelfullpath = modeldir + modelfilename
		
			-- Schrijf naar scnfile
			if(obj.material != undefined and classof(obj.material) != MultiMaterial and obj.material.DiffuseMap != undefined) then (
				
				-- split filename (part after last \) by . to obtain file name without extension
				diffusemapfilename = GetFileName(obj.material.DiffuseMap.Filename)
				tokens = filterString diffusemapfilename "."
				materialfilename = tokens[1] + ".mtl" -- tokens[1] is part before .
				materialfullpath = materialdir + materialfilename 
				
				-- Schrijf matfile
				materialfile = createFile materialfullpath
				if(obj.material.SpecularMap == undefined and obj.material.Bumpmap == undefined) then (
					diffusemappath = datafolder + diffusemapfilename
					format "diffuse % 1\nshader PerPixel" diffusemappath to:materialfile
					
				) else if(obj.material.SpecularMap != undefined and obj.material.Bumpmap == undefined) then (
					diffusemappath = datafolder + diffusemapfilename
					specularmappath = datafolder + GetFileName(obj.material.SpecularMap.Filename)
					format "diffuse % 1\nspecular % 1 1\nshader PerPixelSpecular" diffusemappath specularmappath to:materialfile
					
				) else if(obj.material.SpecularMap == undefined and obj.material.Bumpmap != undefined) then (
					diffusemappath = datafolder + diffusemapfilename
					bumpmappath = datafolder + GetFileName(obj.material.BumpMap.Filename)
					format "diffuse % 1\nnormal %\nshader PerPixelNormal" diffusemappath bumpmappath to:materialfile
					
				) else (
					diffusemappath = datafolder + diffusemapfilename
					specularmappath = datafolder + GetFileName(obj.material.SpecularMap.Filename)
					bumpmappath = datafolder + GetFileName(obj.material.BumpMap.Filename)
					format "diffuse % 1\nspecular % 1 1\nnormal %\nshader PerPixelNormalSpecular" diffusemappath specularmappath bumpmappath to:materialfile
				)
				close materialfile
				
				-- Geef een pad relatief aan Models\ of Materials\ op
				modelfilename = datafolder + modelfilename
				materialfilename = datafolder + materialfilename
				
				-- En exporteren maar
				euler = obj.rotation as eulerangles
				format "AddObject(\"%\",\"%\",%,%,%,%,%,%,%)\n" modelfilename materialfilename obj.pos.x obj.pos.z obj.pos.y euler.x euler.z euler.y obj.scale.x to:scenefile
			)
		)
		close scenefile
	)

	function WriteOBJ = (
		
		-- Paden bijhouden, copy van/naar WriteOBJ
		datafolder = folderprefix.text + "\\"
		scenedir = workingdir + "Scenes\\" + datafolder
		materialdir = workingdir + "Materials\\" + datafolder
		modeldir = workingdir + "Models\\" + datafolder
		
		iterator = 0
		
		makeDir modeldir
		
		exportedmodels = #()
		for obj in geometry do (
			
			-- Vooruitgang
			iterator = iterator+1
			objprogress.value = 100.0f*iterator/geometry.count
			
			-- Kopietje uit WriteOBJ
			modelfilename = obj.name + ".obj"
			modelfullpath = modeldir + modelfilename
			
			-- Hebben we deze al op de schijf opgeslagen? Zo nee, exporteren met correctie
			write = true
			for i = 1 to exportedmodels.count do (
				if(exportedmodels[i] == modelfullpath) then (
					write = false
					break
				)
			)
			if(write) then (
				
				-- handednesscorrectie
			--	addmodifier obj (Mirror())
			--	obj.modifiers[1].mirror_axis = 1
			--	obj.modifiers[1].Mirror_Center.Position = -obj.Position
					
				-- Alles straks netjes achterlaten
				posbackup = obj.pos
				rotationbackup = obj.rotation
				
				-- We moeten zonder transformaties e.d. exporteren
				obj.pos = [0, 0, 0]
				obj.rotation = quat 0 0 0 1

				-- Exporteren met correctie
				select obj
				exportFile(modelfullpath) #noPrompt selectedOnly:true
				
				-- Alles terugzetten
				obj.rotation = rotationbackup
				obj.pos = posbackup
				--	deletemodifier obj 1
				
				append exportedmodels modelfullpath
			)
		)
	)
	
	on exportall pressed do (
		WriteSCN()
		WriteOBJ()
	)
	
	on exportscn pressed do (
		WriteSCN()
	)

	on exportobj pressed do (
		WriteOBJ()
	)
)