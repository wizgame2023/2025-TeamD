using UnityEngine;
using UnityEditor;
using Codice.CM.Common.Workspaces;

[CustomEditor(typeof(ConvertCSV))]
public class ConvertCSVEdit : Editor
{
    public override void OnInspectorGUI()
    {
        //Œ³‚ÌInspector•”•ª‚ð•\Ž¦‚·‚é
        base.OnInspectorGUI();

        ConvertCSV convert = target as ConvertCSV;

        if (GUILayout.Button("Save"))
        {
            convert.Convert();
        }
        if (GUILayout.Button("Load"))
        {
            EditorApplication.delayCall += convert.InputCSV;
        }
        
    }

    
}
