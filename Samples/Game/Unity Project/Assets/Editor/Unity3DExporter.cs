using UnityEditor;
using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

public class Unity3DExporter : EditorWindow
{
	private static bool mIsWindowOpen;
	private bool mExportXmlOnly;
	private bool mIgnoreUnderscore;
	private bool mShowAdvancedSettings;
	private bool mExportStateMaterial = true;
	private bool mDontGenerateMipmapsForLightmaps = true;
	private bool mDiscardTexturesOnBind = true;
    private bool mWriteDataAsBigEndian = true;
    private bool mCombineStaticMeshes = true;
    private bool mWriteColorsAs32Bit = true;
	private string m2ndTextureName;
	private string mPath = "../Data/Scene";

    private List<string> mMeshAssetsProcessed;
	private Dictionary<string, string> mMeshAssetNameToMeshName;
	private Dictionary<string, int> mMeshNameToCount;

	private Dictionary<Texture2D, string> mTextureToName;
	private Dictionary<string, int> mTextureNameToCount;

    private List<string> mMaterialAssetsProcessed;
    private Dictionary<string, string> mMaterialAssetNameToMaterialName;
    private Dictionary<string, int> mMaterialNameToCount;

    private Dictionary<Light, string> mLightToName;
    private Dictionary<string, int> mLightNameToCount;

	private struct Statistics
	{
		public int LightmapsCount;
		public int LightmapsTotalSizeOnDisk;
		public int LightmapsTotalSizeInRam;
		public int TexturesCount;
		public int TexturesTotalSizeOnDisk;
		public int TexturesTotalSizeInRam;

		public void Print()
		{
			Debug.Log("---> Export Statistics <---\n");
			Debug.Log("Textures: " + TexturesCount + " (Lightmaps: " + LightmapsCount + ")\n");
            Debug.Log("Textures total size in RAM: " + (TexturesTotalSizeInRam / (1024.0f * 1024.0f)).ToString("F2") + " MB (Lightmaps: " + (LightmapsTotalSizeInRam / (1024.0f * 1024.0f)).ToString("F2") + " MB)\n"); 
            Debug.Log("Textures total size on disk: " + (TexturesTotalSizeOnDisk / (1024.0f * 1024.0f)).ToString("F2") + " MB (Lightmaps: " + (LightmapsTotalSizeOnDisk / (1024.0f * 1024.0f)).ToString("F2") + " MB)\n");
		}
	}

	private Statistics mStatistics;

	[MenuItem("Wire3D/Exporter")]
	protected static void CreateWizard ()
	{
		if (!mIsWindowOpen) {
			mIsWindowOpen = true;
			Unity3DExporter editor = GetWindow ((typeof(Unity3DExporter))) as Unity3DExporter;
			if (editor) {
				editor.autoRepaintOnSceneChange = true;
			}
		}
	}

    private static List<Transform> GetRootTransforms()
	{
		List<Transform> allTransforms = new List<Transform> (FindObjectsOfType (typeof(Transform)) as Transform[]);
		List<Transform> rootTransforms = allTransforms.Where (t => t.parent == null).ToList ();
		return (from s in rootTransforms orderby s.name select s).ToList ();
	}
	
	private static bool HasRenderObject (Transform transform)
	{
		GameObject gameObject = transform.gameObject;
		MeshFilter meshFilter = gameObject.GetComponent<MeshFilter> ();
		MeshRenderer meshRenderer = gameObject.GetComponent<MeshRenderer> ();
		
		return (meshFilter != null && meshFilter.sharedMesh != null) &&
            (meshRenderer != null && meshRenderer.enabled);
	}
	

	protected void OnDestroy()
	{
		mIsWindowOpen = false;
        if (mTextureToName != null)
        {
            mTextureToName.Clear();
        }

        if (mLightToName != null)
        {
            mLightToName.Clear();
        }
	}

	protected virtual void OnGUI()
	{
        ShowSaveDialog();
		GUILayout.Space(10);

        ShowSettingsDialog();
		GUILayout.Space(10);

        ShowAdvancedSettingsDialog();
		GUILayout.Space(20);

        ShowExportDialog();
	}

    private void ShowSaveDialog()
    {
        EditorGUILayout.BeginHorizontal();
        EditorGUIUtility.LookLikeControls();
        mPath = EditorGUILayout.TextField("Save to directory:", mPath ?? string.Empty);
        EditorGUILayout.EndHorizontal();

        if (string.IsNullOrEmpty(mPath) || !Directory.Exists(mPath))
        {
            Color c = GUI.contentColor;
            GUI.contentColor = new Color(1, 1, 0.1f);
            if (string.IsNullOrEmpty(mPath))
            {
                GUILayout.Label("Enter a destination path for saving the exported data.");
            }
            else
            {
                GUILayout.Label("Specified path '" + mPath + "' does not exist.");
            }

            GUI.contentColor = c;
        }
    }
    
    private void ShowSettingsDialog()
    {
        mExportXmlOnly = GUILayout.Toggle(mExportXmlOnly, new GUIContent("Export scene XML file only", "Textures, meshes, etc. will not be exported."));
        mIgnoreUnderscore = GUILayout.Toggle(mIgnoreUnderscore, "Ignore GameObjects starting with '_'");
    }

    private void ShowAdvancedSettingsDialog()
    {
        mShowAdvancedSettings = EditorGUILayout.Foldout(mShowAdvancedSettings, "Advanced Settings");
        if (!mShowAdvancedSettings)
        {
            return;
        }

        mCombineStaticMeshes = GUILayout.Toggle(mCombineStaticMeshes, new GUIContent(
            "Combine static meshes",
            "Prepares static meshes for drawcall batching."));
        mExportStateMaterial = GUILayout.Toggle(mExportStateMaterial, new GUIContent(
            "Try to export StateMaterial from shader",
            "The 'Main Color', 'Tint Color' or 'Tint' (if available) of a shader will be exported as the StateMaterial's ambient color."));
        mDontGenerateMipmapsForLightmaps = GUILayout.Toggle(mDontGenerateMipmapsForLightmaps, new GUIContent(
            "Do not generate mipmaps for lightmaps",
            "Reduces size of lightmaps by 33%"));
        mDiscardTexturesOnBind = GUILayout.Toggle(mDiscardTexturesOnBind, new GUIContent(
            "Flag textures as 'discard on bind'",
            "Textures that are not discarded at bind time can be bound multiple times, but at the cost of double ram usage per texture."));
        mWriteDataAsBigEndian = GUILayout.Toggle(mWriteDataAsBigEndian, new GUIContent(
            "Write data as big endian",
            "PC uses little endian. Wii uses big endian. Byte order will be rearranged by the Importer if necessary. Thus favoring a particular order gives a slight speedup when importing data on that particular platform."));
        mWriteColorsAs32Bit = GUILayout.Toggle(mWriteColorsAs32Bit, new GUIContent(
            "Write vertex colors as 32bit",
            "Write vertex colors of meshes as 32 bit values instead of 4 floats"));
        GUILayout.Label("Use property names:");
        m2ndTextureName = EditorGUILayout.TextField("- 2nd Texture ", m2ndTextureName ?? string.Empty);
    }

    private void ShowExportDialog()
    {
        if ((Directory.Exists(mPath) && !string.IsNullOrEmpty(mPath)) == false)
        {
            return;
        }

        if (GUILayout.Button("Export"))
        {
            if (mPath[mPath.Length - 1] != '/' && mPath[mPath.Length - 1] != '\\')
            {
                mPath = mPath + '/';
            }

            mMeshAssetsProcessed = new List<string>();
            mMeshAssetNameToMeshName = new Dictionary<string, string>();
            mMeshNameToCount = new Dictionary<string, int>();

            mTextureToName = new Dictionary<Texture2D, string>();
            mTextureNameToCount = new Dictionary<string, int>();

            mMaterialAssetsProcessed = new List<string>();
            mMaterialAssetNameToMaterialName = new Dictionary<string, string>();
            mMaterialNameToCount = new Dictionary<string, int>();

            mLightToName = new Dictionary<Light, string>();
            mLightNameToCount = new Dictionary<string, int>();

            mStatistics = new Statistics();

            Dictionary<MeshFilter, Mesh> backupMeshFilterToMesh = null;
            if (mCombineStaticMeshes)
            {
                CombineStaticMeshes(out backupMeshFilterToMesh);
            }

            Export();

            if (mCombineStaticMeshes)
            {
                UncombineStaticMeshes(backupMeshFilterToMesh);
            }

            if (!mExportXmlOnly)
            {
                mStatistics.Print();
            }
        }
    }

    private static void CombineStaticMeshes(out Dictionary<MeshFilter, Mesh> backupMeshFilterToMesh)
    {
        backupMeshFilterToMesh = new Dictionary<MeshFilter, Mesh>();
        List<Transform> roots = GetRootTransforms();

        foreach (Transform root in roots)
        {
            Stack<Transform> stack = new Stack<Transform>();
            stack.Push(root);
            List<GameObject> gos = new List<GameObject>();

            while (stack.Count > 0)
            {
                Transform t = stack.Pop();
                if (t.gameObject.isStatic)
                {
                    gos.Add(t.gameObject);
                }

                for (int i = t.childCount-1; i >= 0; i--)
                {
                    stack.Push(t.GetChild(i));
                }
            }

            foreach (GameObject go in gos)
            {
                MeshFilter mf = go.GetComponent<MeshFilter>();
                if (mf != null)
                {
                    backupMeshFilterToMesh.Add(mf, mf.sharedMesh);
                }
            }

            StaticBatchingUtility.Combine(gos.ToArray(), root.gameObject);
        }
    }

    private static void UncombineStaticMeshes(Dictionary<MeshFilter, Mesh> backupMeshFilterToMesh)
    {
        if (backupMeshFilterToMesh == null)
        {
            return;
        }

        foreach (KeyValuePair<MeshFilter, Mesh> pair in backupMeshFilterToMesh)
        {
            pair.Key.sharedMesh = pair.Value;
            if (pair.Key.sharedMesh != null)
            {
                MeshRenderer mr = pair.Key.gameObject.GetComponent<MeshRenderer>();
                if (mr == null)
                {
                    continue;
                }

                SerializedObject so = new SerializedObject(mr);
                SerializedProperty sp = so.FindProperty("m_SubsetIndices");
                if (sp != null && sp.isArray)
                {
                    sp.arraySize = 0;
                    so.ApplyModifiedProperties();
                }
            }
        }
    }

	private static string GetUniqueName(string originalName, Dictionary<string, int> context)
	{
        int nameUsageCount = context.ContainsKey(originalName) ? context[originalName] : 0;
		context[originalName] = ++nameUsageCount;

		if (nameUsageCount > 1)
        {
            string newName = originalName + "_" + nameUsageCount + "_(renamed)";
            Debug.LogWarning("Renaming '" + originalName + "' to '" + newName + "'");
			return newName;
		}
        else
        {
			return originalName;
		}
	}

	private void WriteRenderObject(Transform transform, StreamWriter outFile, string indent)
	{
        GameObject go = transform.gameObject;
        MeshFilter meshFilter = go.GetComponent<MeshFilter>();
		MeshRenderer meshRenderer = go.GetComponent<MeshRenderer>();
		
		if ((meshRenderer == null) || (meshFilter == null)) {
			return;
		}

        int subMeshIndex = -1;
        int submeshCount = meshFilter.sharedMesh.subMeshCount;

        SerializedObject so = new SerializedObject(meshRenderer);
        SerializedProperty sp = so.FindProperty("m_SubsetIndices");
        if (sp != null && sp.isArray)
        {
            if (sp.arraySize > 0)
            {
                submeshCount = sp.arraySize;
                subMeshIndex = sp.GetArrayElementAtIndex(0).intValue;
            }
        }

        bool exportSubmeshes = submeshCount > 1 ? true : false;
        if (exportSubmeshes)
        {
            subMeshIndex = 0;
        }

        List<Light> lights = GetLightsForRenderer(meshRenderer);
        bool extraNode = false;
        if (lights.Count > 0 && transform.childCount > 0)
        {
            indent = indent + "  ";
            string isStatic = go.isStatic ? " Static=\"1\"" : "";
            string layer = go.layer == 0 ? "" : " Layer=\"" + go.layer + "\"";
            outFile.WriteLine(indent + "<Node Name=\"" + go.name + "\"" + isStatic + layer + ">");
            extraNode = true;
        }

        foreach (Light light in lights)
        {
            WriteLight(light, outFile, indent);
        }

        if (exportSubmeshes)
        {
            WriteSubLeaves(go, outFile, indent + "  ");
        }
        else
        {
            WriteMesh(meshFilter.sharedMesh, meshRenderer, outFile, indent + "  ", subMeshIndex);
            WriteMaterial(meshRenderer, meshRenderer.sharedMaterial, outFile, indent + "  ");
        }

        if (extraNode)
        {
            outFile.WriteLine(indent + "</Node>");       
        }
	}

    private List<Light> GetLightsForRenderer(Renderer renderer)
    {
        List<Light> lights = new List<Light>();

        int layerMask = 1 << renderer.gameObject.layer;
        foreach (Light light in mLightToName.Keys)
        {
            const int realtimeOnly = 0;
            if (!IsLightmapped(renderer) || (GetLightmapping(light) == realtimeOnly))
            {
                if ((layerMask & light.cullingMask) != 0)
                {
                    lights.Add(light);
                }
            }
        }

        return lights;
    }

    private void WriteSubLeaves(GameObject gameObject, StreamWriter outFile, string indent)
    {
        MeshFilter meshFilter = gameObject.GetComponent<MeshFilter>();
        MeshRenderer meshRenderer = gameObject.GetComponent<MeshRenderer>();

        if ((meshRenderer == null) || (meshFilter == null))
        {
            return;
        }

        Mesh mesh = meshFilter.sharedMesh;
        string isStatic = gameObject.isStatic ? " Static=\"1\"" : "";
        string layer = gameObject.layer == 0 ? "" : " Layer=\"" + gameObject.layer + "\"";
        
        SerializedObject so = new SerializedObject(meshRenderer);
        SerializedProperty sp = so.FindProperty("m_SubsetIndices");
        if (sp != null && sp.isArray)
        {
            if (sp.arraySize != 0)
            {
                for (int j = 0; j < sp.arraySize; j++)
                {
                    int i = sp.GetArrayElementAtIndex(j).intValue;
                    outFile.WriteLine(indent + "<Node Name=\"" + gameObject.name + "\"" + isStatic + layer + ">");
                    WriteMesh(mesh, meshRenderer, outFile, indent + "  ", i);
                    WriteMaterial(meshRenderer, meshRenderer.sharedMaterials[j], outFile, indent + "  ");
                    outFile.WriteLine(indent + "</Node>");
                }
            }
            else
            {
                int submeshCount = mesh.subMeshCount;
                for (int i = 0; i < submeshCount; i++)
                {
                    outFile.WriteLine(indent + "<Node Name=\"" + gameObject.name + "\"" + isStatic + layer + ">");
                    WriteMesh(mesh, meshRenderer, outFile, indent + "  ", i);
                    WriteMaterial(meshRenderer, meshRenderer.sharedMaterials[i], outFile, indent + "  ");
                    outFile.WriteLine(indent + "</Node>");
                }
            }
        }
    }

	private void WriteNode(Transform transform, StreamWriter outFile, string indent)
	{
        GameObject go = transform.gameObject;

        string trafo = GetTransformAsString(transform, go.isStatic);
        string isStatic = "";
        if (go.isStatic)
        {
            if (!trafo.Equals(""))
            {
                isStatic += " ";
            }

            isStatic += "Static=\"1\"";
        }

        bool isEmpty = go.transform.childCount == 0;
        Component[] components = go.GetComponents<Component>();
        isEmpty = isEmpty && (components == null || components.Length < 2);
        string slash = isEmpty ? " /" : "";

        string layer = go.layer == 0 ? "" : " Layer=\"" + go.layer + "\"";

        outFile.WriteLine(indent + "<Node Name=\"" + go.name + "\" " +
             trafo + isStatic + layer + slash + ">");

        WriteLightNode(go.GetComponent<Light>(), outFile, indent);

        WriteCamera(go.GetComponent<Camera>(), outFile, indent);
        WriteColliders(go, outFile, indent);
        WriteRigidbody(go, outFile, indent);

        if (HasRenderObject(transform))
        {
            WriteRenderObject(transform, outFile, indent);
        }

		for (int i = 0; i < transform.childCount; i++)
        {
			Traverse(transform.GetChild (i), outFile, indent + "  ");
		}
    	
        if (!isEmpty)
        {
            outFile.WriteLine(indent + "</Node>");
        }
	}

	private void WriteSkybox (StreamWriter outFile, string indent)
	{
		Material skyboxMaterial = RenderSettings.skybox;
		if (skyboxMaterial == null) {
			return;
		}

		Shader skyBoxShader = skyboxMaterial.shader;
		if (skyBoxShader == null) {
			return;
		}

		if (!skyBoxShader.name.Equals ("RenderFX/Skybox")) {
			return;
		}

		Texture2D posZTexture2D = GetTextureFromMaterial (skyboxMaterial, "_FrontTex");
		Texture2D negZTexture2D = GetTextureFromMaterial (skyboxMaterial, "_BackTex");
		Texture2D posXTexture2D = GetTextureFromMaterial (skyboxMaterial, "_LeftTex");
		Texture2D negXTexture2D = GetTextureFromMaterial (skyboxMaterial, "_RightTex");
		Texture2D posYTexture2D = GetTextureFromMaterial (skyboxMaterial, "_UpTex");
		Texture2D negYTexture2D = GetTextureFromMaterial (skyboxMaterial, "_DownTex");

		outFile.WriteLine (indent + "<Skybox>");

        WriteMaterialState(skyboxMaterial, outFile, indent);

		outFile.WriteLine (indent + "  " + "<PosZ>");
		WriteTexture (posZTexture2D, outFile, indent + "  ");
		outFile.WriteLine (indent + "  " + "</PosZ>");

		outFile.WriteLine (indent + "  " + "<NegZ>");
		WriteTexture (negZTexture2D, outFile, indent + "  ");
		outFile.WriteLine (indent + "  " + "</NegZ>");

		outFile.WriteLine (indent + "  " + "<PosX>");
		WriteTexture (negXTexture2D, outFile, indent + "  ");
		outFile.WriteLine (indent + "  " + "</PosX>");

		outFile.WriteLine (indent + "  " + "<NegX>");
		WriteTexture (posXTexture2D, outFile, indent + "  ");
		outFile.WriteLine (indent + "  " + "</NegX>");

		outFile.WriteLine (indent + "  " + "<PosY>");
		WriteTexture (posYTexture2D, outFile, indent + "  ");
		outFile.WriteLine (indent + "  " + "</PosY>");

		outFile.WriteLine (indent + "  " + "<NegY>");
		WriteTexture (negYTexture2D, outFile, indent + "  ");
		outFile.WriteLine (indent + "  " + "</NegY>");

        outFile.WriteLine(indent + "<FogState Enabled=\"0\" />");

		outFile.WriteLine (indent + "</Skybox>");
	}
    
	static private Texture2D GetTextureFromMaterial (Material material, string propertyName)
	{
		Texture2D texture = null;

		if (material.HasProperty (propertyName)) {
			texture = material.GetTexture (propertyName) as Texture2D;
		}

		return texture;
	}

	private void Traverse(Transform transform, StreamWriter outFile, string indent)
	{
		if (mIgnoreUnderscore && transform.gameObject.name.StartsWith("_"))
        {
			return;
		}
      	
		WriteNode(transform, outFile, indent);
	}
	
	private void Export ()
	{
		string[] unityScenePath = EditorApplication.currentScene.Split (char.Parse ("/"));
		string unitySceneName = unityScenePath[unityScenePath.Length - 1];
		if (string.IsNullOrEmpty (unitySceneName))
        {
			Debug.LogWarning("Scene has no name.\n");
		}
        else
        {
			if (unitySceneName.Substring (unitySceneName.Length - 6, 6).Equals (".unity")) {
				unitySceneName = unitySceneName.Remove(unitySceneName.Length - 6);
			}
		}
        
		StreamWriter outFile = new StreamWriter (mPath + unitySceneName + ".xml");
		try {
            WriteAssets(outFile);

            GameObject root = new GameObject(unitySceneName);
			outFile.WriteLine ("<Node Name=\"" + root.name + "\">");
			DestroyImmediate (root);
			string indent = "  ";

            WriteStateFog (outFile, indent);

			foreach (Transform transform in GetRootTransforms ()) {
				Traverse (transform, outFile, indent);
			}
	
			WriteSkybox (outFile, indent);

			outFile.WriteLine ("</Node>");
		} finally {
			outFile.Close ();
		}
	}

	private string GetTransformAsString(Transform transform, bool useWorldTransform = false)
	{
        GameObject go = transform.gameObject;
        MeshRenderer meshRenderer = go.GetComponent<MeshRenderer>();
        if (meshRenderer != null)
        {
            SerializedObject so = new SerializedObject(meshRenderer);
            SerializedProperty sp = so.FindProperty("m_SubsetIndices");
            if (sp != null && sp.isArray)
            {
                if (sp.arraySize > 0)
                {
                    return string.Empty;
                }
            }
        }

		Vector3 position = transform.localPosition;
		Quaternion rotation = transform.localRotation;
		Vector3 scale = transform.localScale;

        if (useWorldTransform)
        {
            position = transform.position;
            rotation = transform.rotation;
            scale = transform.lossyScale;
        }

        string trafo = "";

        if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
        {
            trafo += "Pos=\"" + (-position.x) + ", " + position.y + ", " + position.z + "\"";
        }

        if (rotation.w != 1.0f || rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
        {
            if (!trafo.Equals(""))
            {
                trafo += " ";
            }

            trafo += "Rot=\"" + rotation.w + ", " + rotation.x + ", " + (-rotation.y) + ", " + (-rotation.z) + "\"";
        }

        if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
        {
            if (!trafo.Equals(""))
            {
                trafo += " ";
            }

            trafo += "Scale=\"" + scale.x + ", " + scale.y + ", " + scale.z + "\"";
        }

        return trafo;
	}

    private void WriteAssets(StreamWriter outFile)
    {
        outFile.WriteLine("<Assets>");
        string indent = "    ";

        WriteLightAssets(outFile, indent);
        WriteMeshAssets(outFile, indent);
        WriteMaterialAssets(outFile, indent);

        outFile.WriteLine("</Assets>");
    }

    private void WriteLightAssets(StreamWriter outFile, string indent)
    {
        bool foundLight = false;
        List<Transform> rootTransforms = GetRootTransforms();
        foreach (Transform transform in rootTransforms)
        {
            if (mIgnoreUnderscore && transform.gameObject.name.StartsWith("_"))
            {
                continue;
            }

            Light[] lights = transform.gameObject.GetComponentsInChildren<Light>();
            foreach (Light light in lights)
            {
                const int bakedOnly = 2;
                if (GetLightmapping(light) != bakedOnly)
                {
                    if (!foundLight)
                    {
                        outFile.WriteLine("  <Lights>");
                        foundLight = true;
                    }

                    WriteLight(light, outFile, indent);
                }
            }
        }

        if (foundLight)
        {
            outFile.WriteLine("  </Lights>");
        }
    }


    private struct MeshAndMeshRenderer
    {
        public Mesh mesh;
        public MeshRenderer renderer;
    }

    private void WriteMeshAssets(StreamWriter outFile, string indent)
    {
        Stack<Transform> stack = new Stack<Transform>();
        foreach (Transform transform in GetRootTransforms())
        {
            stack.Push(transform);
        }

        List<MeshAndMeshRenderer> renderMeshes = new List<MeshAndMeshRenderer>();
        List<MeshAndMeshRenderer> collisionMeshes = new List<MeshAndMeshRenderer>();
        
        while (stack.Count > 0)
        {
            Transform t = stack.Pop();
            if (mIgnoreUnderscore && t.gameObject.name.StartsWith("_"))
            {
                continue;
            }

            for (int i = t.childCount - 1; i >= 0; i--)
            {
                stack.Push(t.GetChild(i));
            }

            MeshCollider meshCollider = t.gameObject.GetComponent<MeshCollider>();
            Mesh collisionMesh = (meshCollider != null) ? meshCollider.sharedMesh : null;
            if (HasRenderObject(t) || collisionMesh != null)
            {
                MeshFilter meshFilter = t.gameObject.GetComponent<MeshFilter>();
                if (meshFilter == null && collisionMesh == null)
                {
                    continue;
                }

                MeshRenderer meshRenderer = t.gameObject.GetComponent<MeshRenderer>();
                Mesh renderMesh = meshFilter != null ? meshFilter.sharedMesh : null; 

                if (meshFilter != null)
                {
                    MeshAndMeshRenderer mmr = new MeshAndMeshRenderer();
                    mmr.mesh = renderMesh;
                    mmr.renderer = meshRenderer;
                    renderMeshes.Add(mmr);
                }

                if (collisionMesh != null && collisionMesh != renderMesh)
                {
                    MeshAndMeshRenderer mmr = new MeshAndMeshRenderer();
                    mmr.mesh = collisionMesh;
                    mmr.renderer = null;
                    collisionMeshes.Add(mmr);
                }
            }
        }

        bool foundAsset = renderMeshes.Count > 0 || collisionMeshes.Count > 0;
        if (foundAsset)
        {
            outFile.WriteLine("  <Meshes>");
        }

        foreach (MeshAndMeshRenderer mmr in renderMeshes)
        {
            WriteMeshAsset(mmr.mesh, mmr.renderer, outFile, indent);
        }

        foreach (MeshAndMeshRenderer mmr in collisionMeshes)
        {
            WriteMeshAsset(mmr.mesh, mmr.renderer, outFile, indent, true);
        }

        if (foundAsset)
        {
            outFile.WriteLine("  </Meshes>");
        }
    }

    private void WriteMaterialAssets(StreamWriter outFile, string indent)
    {
        Stack<Transform> stack = new Stack<Transform>();
        foreach (Transform transform in GetRootTransforms())
        {
            stack.Push(transform);
        }

        bool foundAsset = false;
        while (stack.Count > 0)
        {
            Transform t = stack.Pop();
            if (mIgnoreUnderscore && t.gameObject.name.StartsWith("_"))
            {
                continue;
            }

            for (int i = t.childCount-1; i >= 0; i--)
            {
                stack.Push(t.GetChild(i));
            }

            if (HasRenderObject(t))
            {
                MeshRenderer meshRenderer = t.gameObject.GetComponent<MeshRenderer>();
                foreach (Material material in meshRenderer.sharedMaterials)
                {
                    string materialAssetName = GetMaterialAssetName(material, meshRenderer);
                    if (!mMaterialAssetsProcessed.Contains(materialAssetName))
                    {
                        if (!foundAsset)
                        {
                            foundAsset = true;
                            outFile.WriteLine("  <Materials>");
                        }

                        WriteMaterial(meshRenderer, material, outFile, indent);
                    }
                }
            }
        }

        if (foundAsset)
        {
            outFile.WriteLine("  </Materials>");
        }
    }

    private void WriteMeshAsset(Mesh mesh, MeshRenderer meshRenderer, StreamWriter outFile, string indent,
        bool isCollisionMesh = false)
    {
        if (mesh != null)
        {
            string meshAssetName = GetMeshAssetName(mesh, GetLightmapTilingOffset(meshRenderer));
            if (!mMeshAssetsProcessed.Contains(meshAssetName))
            {
                WriteMesh(mesh, meshRenderer, outFile, indent, -1, isCollisionMesh);
            }
        }
    }
    
    private Vector4 GetLightmapTilingOffset(MeshRenderer meshRenderer)
    {
        if (meshRenderer == null)
        {
            return new Vector4(1, 1, 0, 0);
        }

        // combined meshes already have lightmap tiling and offset applied
        Vector4 lightmapTilingOffset = meshRenderer.lightmapTilingOffset;
        SerializedObject so = new SerializedObject(meshRenderer);
        SerializedProperty sp = so.FindProperty("m_SubsetIndices");
        if (sp != null && sp.isArray)
        {
            if (sp.arraySize != 0)
            {
                lightmapTilingOffset = new Vector4(1, 1, 0, 0);
            }
        }

        return lightmapTilingOffset;
    }

    private void WriteStateFog (StreamWriter outFile, string indent)
    {
        if (!RenderSettings.fog)
        {
            return;
        }

        string enabled = RenderSettings.fog ? "1" : "0";
        Color color = RenderSettings.fogColor;
        string mode = "LINEAR";
        if (RenderSettings.fogMode == FogMode.Exponential)
        {
            mode = "EXP";
        }
        else if (RenderSettings.fogMode == FogMode.ExponentialSquared)
        {
            mode = "EXPSQR";
        }

        outFile.WriteLine(indent + "<FogState Enabled=\"" + enabled + "\" Color=\"" + color.r + ", " + color.g + ", " + color.b + "\" " +
            "Start=\"" + RenderSettings.fogStartDistance + "\" End=\"" + RenderSettings.fogEndDistance + "\"" + " Mode=\"" + mode + "\" />");
    }

    private void WriteLightNode(Light light, StreamWriter outFile, string indent)
    {
        if (light == null)
        {
            return;
        }

        outFile.WriteLine(indent + "  " + "<LightNode>");
        WriteLight(light, outFile, indent + "  ");
        outFile.WriteLine(indent + "  " + "</LightNode>");
    }
    
    private void WriteLight(Light light, StreamWriter outFile, string indent)
	{
		if (light == null)
        {
			return;
		}

        bool alreadyProcessed = true;
        if (!mLightToName.ContainsKey(light))
        {
            mLightToName.Add(light, GetUniqueName(light.name, mLightNameToCount));
            alreadyProcessed = false;
        }

        string lightName = mLightToName[light];
        
        Color ambient = RenderSettings.ambientLight;
		Color color = light.color * light.intensity;

        string lightmap = string.Empty;
        const int realtimeOnly = 0;
        if (GetLightmapping(light) == realtimeOnly)
        {
            ambient = new Color(0, 0, 0, 0);
        }
        else
        {
            lightmap = " Baked=\"1\"";
        }

        if (alreadyProcessed)
        {
            outFile.WriteLine(indent + "  " + "<Light Name=\"" + lightName + "\" />");
        }
        else
        {
            string direction = string.Empty;
            if (light.type != LightType.Point)
            {
                direction = " Direction=\"0, 0, 1\"";
            }

            string range = light.type != LightType.Directional ? " Range=\"" + light.range + "\"" : "";
            string mask = light.cullingMask == ~0 ? "" : " Mask=\"" + light.cullingMask.ToString("X") + "\"";
            string enabled = light.enabled ? "" : " Enabled=\"0\"";
            
            outFile.WriteLine(indent + "  " + "<Light Name=\"" + lightName + "\" Type=\"" + light.type +
                "\"" + direction + range + " Ambient=\"" + ambient.r + ", " + ambient.g + ", " + ambient.b +
			    "\" Color=\"" + color.r + ", " + color.g + ", " + color.b + "\"" + mask + enabled + lightmap + " />");
        }
	}

    private void WriteColliders(GameObject gameObject, StreamWriter outFile, string indent)
    {
        Collider[] colliders = gameObject.GetComponents<Collider>();
        foreach (Collider collider in colliders)
        {
            WriteCollider(collider, gameObject, outFile, indent);
        }
    }

	private void WriteCollider(Collider collider, GameObject gameObject, StreamWriter outFile, string indent)
	{
		if (collider == null)
        {
			return;
		}
		
        string shape = GetColliderShapeName(collider);
        string isEnabled = collider.enabled ? string.Empty : "Enabled=\"0\" ";
		outFile.Write (indent + "  " + "<Collider " + isEnabled + "Shape=\"" + shape + "\" ");
		
		if (collider is BoxCollider)
        {
			WriteBoxColliderAttributes(collider as BoxCollider, outFile, indent);
		}
        else if (collider is SphereCollider)
        {
            WriteSphereColliderAttributes(collider as SphereCollider, outFile, indent);
        }
        else if (collider is CapsuleCollider)
        {
            WriteCapsuleColliderAttributes(collider as CapsuleCollider, outFile, indent);
        }
        else if (collider is MeshCollider)
        {
            WriteMeshColliderAttributes(collider as MeshCollider, outFile, indent);
        }
        else if (collider is CharacterController)
        {
            WriteCharacterControllerAttributes(collider as CharacterController, outFile, indent);
        }
        else
        {
			Debug.Log("Collider shape not supported: '" + shape + "'.", gameObject);
		}

        Transform parent = gameObject.transform.parent;
        if (parent != null && parent.lossyScale != Vector3.one)
        {
            Debug.LogWarning("'" + gameObject.name + "' has one or more parent transforms with scale. " +
            "This causes incorrect collision shapes.", gameObject);
        }

		outFile.Write (" />\n");
	}
	
	private string GetColliderShapeName (Collider collider)
	{
		string typeName = collider.GetType ().Name;
        typeName = typeName.Replace("Controller", "");
		return typeName.Replace ("Collider", "");
	}

    private void WriteCharacterControllerAttributes(CharacterController characterController, StreamWriter outFile, string indent)
    {
        Vector3 center = characterController.center;
        float height = characterController.height;
        height = height - 2 * characterController.radius;
        height = height < 0 ? 0 : height;
        outFile.Write("Height=\"" + height + "\" Radius=\"" + characterController.radius + "\" Slope=\"" + characterController.slopeLimit +
            "\" Step=\"" + characterController.stepOffset + "\" Center=\"" + (-center.x) + ", " + center.y + ", " + center.z + "\"");
    }

	private void WriteBoxColliderAttributes (BoxCollider boxCollider, StreamWriter outFile, string indent)
	{
		Vector3 center = boxCollider.center;
		Vector3 size = boxCollider.size;
		
		outFile.Write ("Center=\"" + (-center.x) + ", " + center.y + ", " + center.z +
			"\" Size=\"" + size.x + ", " + size.y + ", " + size.z + "\"");
	}

    private void WriteSphereColliderAttributes(SphereCollider sphereCollider, StreamWriter outFile, string indent)
    {
        Vector3 center = sphereCollider.center;
        float radius = sphereCollider.radius;

        outFile.Write("Center=\"" + (-center.x) + ", " + center.y + ", " + center.z + "\" Radius=\"" + radius + "\"");
    }

    private void WriteCapsuleColliderAttributes(CapsuleCollider capsuleCollider, StreamWriter outFile, string indent)
    {
        Vector3 center = capsuleCollider.center;
        float radius = capsuleCollider.radius;
        float height = capsuleCollider.height;
        height = height - 2 * radius;
        height = height < 0 ? 0 : height;
        char direction = capsuleCollider.direction < 1 ? 'X' : 'Y';
        direction = capsuleCollider.direction > 1 ? 'Z' : direction;

        outFile.Write("Center=\"" + (-center.x) + ", " + center.y + ", " + center.z + "\" Radius=\"" + radius + "\"" +
            " Height=\"" + height + "\"" + " Direction=\"" + direction + "\"");
    }

    private void WriteMeshColliderAttributes(MeshCollider meshCollider, StreamWriter outFile, string indent)
    {
        string convex = meshCollider.convex ? " Convex=\"1\"" : "";
        outFile.Write("Mesh=\"" + meshCollider.sharedMesh.name + "\"" + convex);
    }

    private void WriteRigidbody(GameObject gameObject, StreamWriter outFile, string indent)
    {
        Rigidbody rigidbody = gameObject.GetComponent<Rigidbody>();
        if (rigidbody == null)
        {
            return;
        }

        string kinematic = rigidbody.isKinematic ? " Kinematic=\"1\"" : "";

        // TODO: find a way to map PhysX values to Bullet values
        string damping = rigidbody.drag != 0 ? " Drag=\"" + rigidbody.drag + "\"" : "";
        string angularDamping = " AngularDrag=\"" + rigidbody.angularDrag + "\"";

        outFile.Write(indent + "  " + "<RigidBody Mass=\"" + rigidbody.mass + "\"" +
            kinematic + damping + angularDamping + " />\n");
    }

    private void WriteCamera(Camera camera, StreamWriter outFile, string indent)
	{
		if (camera == null) {
			return;
		}

		float fieldOfView = camera.fieldOfView;
		if (camera.orthographic) {
			fieldOfView = 0;
		}

        string viewport = string.Empty;
        float left = camera.rect.xMin;
        float bottom = camera.rect.yMin;
        float right = camera.rect.xMax;
        float top = camera.rect.yMax;

        right = right > 1 ? 1 : right;
        top = top > 1 ? 1 : top;

        if (left != 0 || right != 1 || bottom != 0 || top != 1)
        {
            viewport = "Left=\"" + left + "\" Right=\"" + right + "\" Top=\"" + top + "\" Bottom=\"" + bottom + "\" ";
        }

        string mask = camera.cullingMask == ~0 ? "" : " Mask=\"" + camera.cullingMask.ToString("X") + "\" ";

		outFile.WriteLine (indent + "  " + "<Camera Fov=\"" + fieldOfView + "\" Near=\"" +
            camera.nearClipPlane + "\" Far=\"" + camera.farClipPlane + "\" " + viewport + mask + "/>");
	}

    private string GetMaterialName(Material material, MeshRenderer meshRenderer)
    {
        string lightMapIndex = string.Empty;
        if (meshRenderer.lightmapIndex != -1)
        {
            lightMapIndex += "_" + meshRenderer.lightmapIndex;
        }

        List<Light> lights = GetLightsForRenderer(meshRenderer);
        if (lights.Count > 0)
        {
            lightMapIndex += "_lit";
        }

        return material.name + lightMapIndex;
    }

    private string GetMaterialAssetName(Material material, MeshRenderer meshRenderer)
    {
        return GetMaterialName(material, meshRenderer) + "_" + material.GetInstanceID().ToString("X8");
    }

    private void WriteMaterial(MeshRenderer meshRenderer, Material material, StreamWriter outFile, string indent)
	{
		if (material == null)
        {
			return;
		}

        string materialName = GetMaterialName(material, meshRenderer);
        string materialAssetName = GetMaterialAssetName(material, meshRenderer);
        bool alreadyProcessed = true;
        if (!mMaterialAssetsProcessed.Contains(materialAssetName))
        {
            mMaterialAssetsProcessed.Add(materialAssetName);
            mMaterialAssetNameToMaterialName.Add(materialAssetName, GetUniqueName(materialName, mMaterialNameToCount));
            alreadyProcessed = false;
        }

        materialName = mMaterialAssetNameToMaterialName[materialAssetName];

        if (alreadyProcessed)
        {
            outFile.WriteLine(indent + "<Material Name=\"" + materialName + "\" />");
            return;
        }
        else
        {
            outFile.WriteLine(indent + "<Material Name=\"" + materialName + "\">");
        }

		Texture2D texture = material.mainTexture as Texture2D;

        bool usesLightmap = meshRenderer.lightmapIndex != -1 && meshRenderer.lightmapIndex != 254;
        bool isRealtimeLit = GetLightsForRenderer(meshRenderer).Count > 0;

        if (usesLightmap)
        {
            Texture2D lightmap = LightmapSettings.lightmaps[meshRenderer.lightmapIndex].lightmapFar;
            WriteTexture(lightmap, outFile, indent, true, isRealtimeLit);
        }

        WriteTexture(texture, outFile, indent, false, isRealtimeLit);

		if (!string.IsNullOrEmpty(m2ndTextureName) && material.HasProperty(m2ndTextureName))
        {
			Texture2D _2ndTexture = material.GetTexture(m2ndTextureName) as Texture2D;
			WriteTexture(_2ndTexture, outFile, indent);
		}

		WriteMaterialState(material, outFile, indent);

		outFile.WriteLine(indent + "</Material>");
	}

	private void WriteMaterialState(Material materialState, StreamWriter outFile, string indent)
	{
		if (mExportStateMaterial == false)
        {
			return;
		}

		if (materialState.HasProperty("_Color"))
        {
			Color color = materialState.GetColor("_Color");
			
			outFile.WriteLine (indent + "  " + "<MaterialState Ambient=\"" +
                color.r + ", " + color.g + ", " + color.b + ", " + color.a + "\" />");
		}
        else if (materialState.HasProperty("_TintColor"))
        {
            Color color = materialState.GetColor("_TintColor");

            outFile.WriteLine(indent + "  " + "<MaterialState Ambient=\"" +
                color.r + ", " + color.g + ", " + color.b + ", " + color.a + "\" />");
        }
        else if (materialState.HasProperty("_Tint"))
        {
            Color color = materialState.GetColor("_Tint");

            outFile.WriteLine(indent + "  " + "<MaterialState Ambient=\"" +
                color.r + ", " + color.g + ", " + color.b + ", " + color.a + "\" />");
        }
	}

	private void WriteTexture(Texture2D texture, StreamWriter outFile, string indent, bool isLightmap = false, bool isRealtimeLit = false)
	{
		if (texture == null)
        {
			return;
		}

		bool alreadyProcessed = true;
        if (!mTextureToName.ContainsKey(texture))
        {
            mTextureToName.Add(texture, GetUniqueName(texture.name, mTextureNameToCount));
			alreadyProcessed = false;
		}

		string texName = mTextureToName[texture] + ".png";

		int mipmapCount = (mDontGenerateMipmapsForLightmaps && isLightmap) ? 1 : texture.mipmapCount;

		string textureXmlNode = indent + "  <Texture Name=\"" + texName +
            "\" FilterMode=\"" + texture.filterMode + "\" AnisoLevel=\"" + texture.anisoLevel +
            "\" WrapMode=\"" + texture.wrapMode + "\" Mipmaps=\"" + mipmapCount + "\" ";

        if (isRealtimeLit)
        {
            if (isLightmap)
            {
                textureXmlNode += "BlendMode=\"Add\" ";
            }
            else
            {
                textureXmlNode += "BlendMode=\"Modulate\" ";
            }
        }

		if (mDiscardTexturesOnBind)
        {
			textureXmlNode += "Usage=\"STATIC_DISCARD_ON_BIND\" ";
		}

		textureXmlNode += "/>";

		outFile.WriteLine (textureXmlNode);

        if (!alreadyProcessed)
        {
            SaveImage(texture, isLightmap);
        }
    }

    private void SaveImage(Texture2D texture, bool isLightmap)
    {
        if (mExportXmlOnly)
        {
            return;
        }

        string assetPath = AssetDatabase.GetAssetPath(texture);
		TextureImporter textureImporter = AssetImporter.GetAtPath (assetPath) as TextureImporter;
		if (textureImporter == null)
        {
			Debug.Log("Error getting TextureImporter for '" + texture.name + "'");
			return;
		}

		bool wasReadable = textureImporter.isReadable;
		TextureImporterFormat wasFormat = textureImporter.textureFormat;

		bool needsReimport = false;

		if (wasReadable == false)
        {
			needsReimport = true;
			textureImporter.isReadable = true;
		}

		if (texture.format == TextureFormat.DXT1)
        {
			needsReimport = true;
			textureImporter.textureFormat = TextureImporterFormat.RGB24;
		}

		if (texture.format == TextureFormat.DXT5 || texture.format == TextureFormat.RGBA32)
        {
			needsReimport = true;
			textureImporter.textureFormat = TextureImporterFormat.ARGB32;
		}

		if (needsReimport)
        {
			AssetDatabase.ImportAsset (assetPath);
		}

        string texName = mTextureToName[texture] + ".png";
        int mipmapCount = (mDontGenerateMipmapsForLightmaps && isLightmap) ? 1 : texture.mipmapCount;
        int bpp = textureImporter.DoesSourceTextureHaveAlpha() ? 4 : 3;
		bpp = isLightmap ? 3 : bpp;

		mStatistics.TexturesTotalSizeInRam += GetSizeFromLevelOfMipmaps (mipmapCount, texture) * bpp;
		mStatistics.TexturesCount++;

		if (!isLightmap)
        {
			Byte[] bytes = texture.EncodeToPNG();
			File.WriteAllBytes (mPath + "/" + texName, bytes);
			mStatistics.TexturesTotalSizeOnDisk += bytes.Length;
		}
        else
        {
			if (texture.format == TextureFormat.ARGB32)
            {
				Color32[] texSrc = texture.GetPixels32 ();

				Texture2D texRGB = new Texture2D (texture.width, texture.height, TextureFormat.RGB24, false);
				Color32[] texDst = texRGB.GetPixels32 ();

				for (int k = 0; k < texSrc.Length; k++) {
					float[] c = new float[4];
					c [0] = texSrc [k].a / 255.0f;
					c [1] = (texSrc [k].r / 255.0f) * 8.0f * c [0];
					c [2] = (texSrc [k].g / 255.0f) * 8.0f * c [0];
					c [3] = (texSrc [k].b / 255.0f) * 8.0f * c [0];

					c [1] = c [1] > 1.0f ? 1.0f : c [1];
					c [2] = c [2] > 1.0f ? 1.0f : c [2];
					c [3] = c [3] > 1.0f ? 1.0f : c [3];

					texDst [k].a = 255;// (byte)(c[0] * 255.0f);
					texDst [k].r = (byte)(c [1] * 255.0f);
					texDst [k].g = (byte)(c [2] * 255.0f);
					texDst [k].b = (byte)(c [3] * 255.0f);
				}

				texRGB.SetPixels32 (texDst);
				texRGB.Apply();

				Byte[] bytes = texRGB.EncodeToPNG();
				File.WriteAllBytes (mPath + "/" + texName, bytes);
				mStatistics.LightmapsCount++;
				mStatistics.LightmapsTotalSizeOnDisk += bytes.Length;
				mStatistics.TexturesTotalSizeOnDisk += bytes.Length;
				mStatistics.LightmapsTotalSizeInRam += GetSizeFromLevelOfMipmaps (mipmapCount, texture) * bpp;
			} else {
				Debug.Log ("Lightmap '" + texture.name + "'could not be read as ARGB32.");
			}
		}

		if (needsReimport)
        {
			textureImporter.isReadable = wasReadable;
			textureImporter.textureFormat = wasFormat;
			AssetDatabase.ImportAsset (assetPath);
		}
	}

    private string GetMeshAssetName(Mesh mesh, Vector4 lightmapTilingOffset)
    {
        string lightmapPostfix = "_" + lightmapTilingOffset.x + "_" + lightmapTilingOffset.y + "_" +
            lightmapTilingOffset.z + "_" + lightmapTilingOffset.w;

        return mesh.name + lightmapPostfix + "_" + mesh.GetInstanceID().ToString("X8");
    }

    private bool IsLightmapped(Renderer renderer)
    {
        GameObject go = renderer != null ? renderer.gameObject : null;
        return go && (renderer.lightmapIndex < 254 && renderer.lightmapIndex != -1);
    }

    private int GetLightmapping(Light light)
    {
        SerializedObject serialObj = new SerializedObject(light);
        SerializedProperty lightmapProp = serialObj.FindProperty("m_Lightmapping");
        return lightmapProp.intValue;
    }

	private void WriteMesh(Mesh mesh, MeshRenderer meshRenderer, StreamWriter outFile, string indent,
        int subMeshIndex = -1, bool isCollisionMesh = false)
	{
		if (mesh == null)
        {
			return;
		}

        Vector4 lightmapTilingOffset = GetLightmapTilingOffset(meshRenderer);
        string meshName = mesh.name;
        string meshAssetName = GetMeshAssetName(mesh, lightmapTilingOffset);
		bool alreadyProcessed = true;
        if (!mMeshAssetsProcessed.Contains(meshAssetName))
        {
            mMeshAssetsProcessed.Add(meshAssetName);
            string uniqueMeshName = GetUniqueName(meshName, mMeshNameToCount);
            uniqueMeshName = uniqueMeshName.Replace("root:", "root");
            mMeshAssetNameToMeshName.Add(meshAssetName, uniqueMeshName);
			alreadyProcessed = false;
		}

        meshName = mMeshAssetNameToMeshName[meshAssetName];

        string subMesh = subMeshIndex == -1 ? "" : " SubMesh=\"" + subMeshIndex + "\"";

        if (alreadyProcessed)
        {
            outFile.WriteLine(indent + "<Mesh Name=\"" + meshName + "\"" + subMesh + " />");
            return;
        }

        outFile.WriteLine(indent + "<Mesh Name=\"" + meshName + "\"" + subMesh + ">");

        if (mesh.subMeshCount > 1)
        {
            outFile.WriteLine (indent + "  <SubMeshes>");

            int startIndex = 0;
            
            for (int i = 0; i < mesh.subMeshCount; i++)
            {
                int indexCount = mesh.GetTriangles(i).Length;
                outFile.WriteLine (indent + "    " + "<SubMesh Index=\"" + i + "\" StartIndex=\"" + startIndex +
                    "\" IndexCount=\"" + indexCount + "\" />");
                startIndex += indexCount;
            }

            outFile.WriteLine (indent + "  </SubMeshes>");
        }

		string le = mWriteDataAsBigEndian ? string.Empty : " LittleEndian=\"1\"";

        string vtxName = meshName + ".vtx";
		outFile.WriteLine (indent + "  <Vertices Name=\"" + vtxName + "\"" + le + " />");
		if (!alreadyProcessed) {
			SaveVector3s (mesh.vertices, vtxName);
		}

        bool is16Bit = true;
        int[] triangles = mesh.triangles;
        foreach (int i in triangles)
        {
            if (i > 65535)
            {
                is16Bit = false;
                break;
            }
        }

        if (!is16Bit)
        {
            Debug.LogError("Mesh '" + mesh.name + "' uses 32bit indices.");
        }

        string is16BitString = is16Bit ? " 16bit=\"1\"" : string.Empty;
        string idxName = meshName + ".idx";
        outFile.WriteLine(indent + "  <Indices Name=\"" + idxName + "\"" + le + is16BitString + " />");
		if (!alreadyProcessed) {
            SaveIndices(mesh.triangles, idxName, is16Bit);
		}

		if (mesh.normals.Length > 0 && !isCollisionMesh)
        {
            string nmName = meshName + ".nm";
			outFile.WriteLine (indent + "  <Normals Name=\"" + nmName + "\"" + le + " />");
			if (!alreadyProcessed) {
				SaveVector3s (mesh.normals, nmName);
			}
		}

        if (mesh.colors.Length > 0 && !isCollisionMesh)
        {
            string is32BitString = mWriteColorsAs32Bit ? " 32bit=\"1\"" : string.Empty;
            string colName = meshName + ".col";
			outFile.WriteLine (indent + "  <Colors Name=\"" + colName + "\"" + le + is32BitString + " />");
			if (!alreadyProcessed) {
				SaveColors (mesh, colName);
			}
		}

        if (IsLightmapped(meshRenderer))
        {
            if (mesh.uv2.Length > 0 && !isCollisionMesh)
            {
                string uv1Name = meshName + ".uv0";
                outFile.WriteLine(indent + "  <Uv0 Name=\"" + uv1Name + "\"" + le + " />");
                if (!alreadyProcessed)
                {
                    SaveVector2s(mesh.uv2, uv1Name, lightmapTilingOffset);
                }
            }

            if (mesh.uv.Length > 0 && !isCollisionMesh)
            {
                bool hasUv2 = mesh.uv2.Length > 0;
                string uv0Name = meshName + (hasUv2 ? ".uv1" : ".uv0");
                string uvTag = hasUv2 ? "  <Uv1 Name=\"" : "  <Uv0 Name=\"";
                outFile.WriteLine(indent + uvTag + uv0Name + "\"" + le + " />");
                if (!alreadyProcessed)
                {
                    SaveVector2s(mesh.uv, uv0Name, new Vector4(1, 1, 0, 0));
                }
            }
        }
        else
        {
            if (mesh.uv.Length > 0 && !isCollisionMesh)
            {
                string uv0Name = meshName + ".uv0";
                outFile.WriteLine(indent + "  <Uv0 Name=\"" + uv0Name + "\"" + le + " />");
                if (!alreadyProcessed)
                {
                    SaveVector2s(mesh.uv, uv0Name, new Vector4(1, 1, 0, 0));
                }
            }

            if (mesh.uv2.Length > 0 && !isCollisionMesh)
            {
                string uv1Name = meshName + ".uv1";
                outFile.WriteLine(indent + "  <Uv1 Name=\"" + uv1Name + "\"" + le + " />");
                if (!alreadyProcessed)
                {
                    SaveVector2s(mesh.uv2, uv1Name, lightmapTilingOffset);
                }
            }
        }
        
       
		outFile.WriteLine (indent + "</Mesh>");
	}

	private void SaveVector3s (Vector3[] vectors, string name)
	{
		if (mExportXmlOnly) {
			return;
		}
		
		FileStream fileStream = new FileStream (mPath + "/" + name, FileMode.Create);
		BinaryWriter binaryWriter = new BinaryWriter (fileStream);

		for (int i = 0; i < vectors.Length; i++)
        {
            Vector3 vec = vectors[i];
            vec.x = -vec.x;

            WriteFloat(vec.x, binaryWriter);
            WriteFloat(vec.y, binaryWriter);
            WriteFloat(vec.z, binaryWriter);
        }

		binaryWriter.Close ();
		fileStream.Close ();
	}

	private void SaveVector2s (Vector2[] vectors, string name, Vector4 lightmapTilingOffset)
	{
		if (mExportXmlOnly) {
			return;
		}

		FileStream fileStream = new FileStream (mPath + "/" + name, FileMode.Create);
		BinaryWriter binaryWriter = new BinaryWriter (fileStream);

		Vector2 scale = new Vector2 (lightmapTilingOffset.x, lightmapTilingOffset.y);
		Vector2 offset = new Vector2 (lightmapTilingOffset.z, lightmapTilingOffset.w);

		for (int i = 0; i < vectors.Length; i++)
        {
			Vector2 uv = offset + new Vector2 (scale.x * vectors [i].x, scale.y * vectors [i].y);
            uv.y = 1.0f - uv.y; // OpenGL vs DirectX convention

            WriteFloat(uv.x, binaryWriter);
            WriteFloat(uv.y, binaryWriter);
		}

		binaryWriter.Close ();
		fileStream.Close ();
	}

	private void SaveColors (Mesh mesh, string name)
	{
		if (mExportXmlOnly) {
			return;
		}

		FileStream fileStream = new FileStream (mPath + "/" + name, FileMode.Create);
		BinaryWriter binaryWriter = new BinaryWriter (fileStream);

        if (mWriteColorsAs32Bit)
        {
            Color32[] colors = mesh.colors32;
            for (int i = 0; i < colors.Length; i++)
            {
                binaryWriter.Write(colors[i].r);
                binaryWriter.Write(colors[i].g);
                binaryWriter.Write(colors[i].b);
                binaryWriter.Write(colors[i].a);
            }
        }
        else
        {
            Color[] colors = mesh.colors;
            for (int i = 0; i < colors.Length; i++)
            {
                WriteFloat(colors[i].r, binaryWriter);
                WriteFloat(colors[i].g, binaryWriter);
                WriteFloat(colors[i].b, binaryWriter);
                WriteFloat(colors[i].a, binaryWriter);
            }
        }

		binaryWriter.Close ();
		fileStream.Close ();       
	}

    private void WriteFloat(float f, BinaryWriter writer)
    {
        if ((BitConverter.IsLittleEndian && mWriteDataAsBigEndian) ||
            (!BitConverter.IsLittleEndian && !mWriteDataAsBigEndian))
        {
            byte[] x = BitConverter.GetBytes(f);
            Array.Reverse(x);
            writer.Write(x);
        }
        else
        {
            writer.Write(f);
        }
    }

    private void SaveIndices(int[] indices, string name, bool is16Bit)
	{
		if (mExportXmlOnly) {
			return;
		}

		FileStream fileStream = new FileStream (mPath + "/" + name, FileMode.Create);
		BinaryWriter binaryWriter = new BinaryWriter (fileStream);

        if (is16Bit)
        {
            for (int i = 0; i < indices.Length; i++)
            {
                ushort s = (ushort)indices[i];
                if ((BitConverter.IsLittleEndian && mWriteDataAsBigEndian) ||
                    (!BitConverter.IsLittleEndian && !mWriteDataAsBigEndian))
                {
                    byte[] x = BitConverter.GetBytes(s);
                    Array.Reverse(x);
                    binaryWriter.Write(x);
                }
                else
                {
                    binaryWriter.Write(s);
                }
            }
        }
        else
        {
            for (int i = 0; i < indices.Length; i++)
            {
                if ((BitConverter.IsLittleEndian && mWriteDataAsBigEndian) ||
                    (!BitConverter.IsLittleEndian && !mWriteDataAsBigEndian))
                {
                    byte[] x = BitConverter.GetBytes(indices[i]);
                    Array.Reverse(x);
                    binaryWriter.Write(x);
                }
                else
                {
                    binaryWriter.Write(indices[i]);
                }
            }
        }

		binaryWriter.Close ();
		fileStream.Close ();
	}

	private static int GetSizeFromLevelOfMipmaps (int mipmapCount, Texture2D texture)
	{
		if (mipmapCount == 1) {
			return texture.width * texture.height;
		}

		int size = 0;
		for (int i = 0; i < mipmapCount; i++) {
			int n = 1 << i;
			size += (n * n);
		}

		return size;
	}
}
