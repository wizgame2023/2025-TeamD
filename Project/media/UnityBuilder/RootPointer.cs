using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[ExecuteAlways]
public class RootPointer : MonoBehaviour
{
    public string className; 
    public List<GameObject> pointer = new List<GameObject>();
    public int number;

    [HideInInspector]
    public string loadPointer;

    private void Update()
    {
        var obj = UnityEditor.Selection.gameObjects;
        bool isSelect = false;
        foreach(var objObj in obj)
        {
            if(objObj == this.gameObject)
            {
                isSelect = true;
            }
        }
        if (isSelect)
        {
            foreach (var point in pointer)
            {
                Debug.DrawRay(transform.position, (point.transform.position - transform.position));
            }
        }
    }

    private void OnRenderObject()
    {
        //Update等の更新処理を実行させる
        EditorApplication.QueuePlayerLoopUpdate();

        //Sceneの描画更新
        SceneView.RepaintAll();
    }
}
