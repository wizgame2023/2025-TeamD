using Unity.VisualScripting;
using UnityEditor;
using UnityEngine;
[CustomEditor(typeof(RootPointer))]
public class PointerEdit : Editor
{
    public override void OnInspectorGUI()
    {
        //Œ³‚ÌInspector•”•ª‚ð•\Ž¦‚·‚é
        base.OnInspectorGUI();
        
        RootPointer pointer = target as RootPointer;
        
        if (GUILayout.Button("Add Root"))
        {
            var objects = UnityEditor.Selection.objects;
            foreach (var obj in objects)
            {
                GameObject gameObject = obj as GameObject;
                if(gameObject.GetComponent<RootPointer>() != null)
                {
                    pointer.pointer.Add(gameObject);
                }
            }
        }
        if (GUILayout.Button("new Root"))
        {
            pointer.pointer.Clear();
            var objects = UnityEditor.Selection.objects;
            foreach (var obj in objects)
            {
                GameObject gameObject = obj as GameObject;
                if (gameObject.GetComponent<RootPointer>() != null)
                {
                    pointer.pointer.Add(gameObject);
                }
            }
        }
    }
}
