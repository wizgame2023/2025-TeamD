using JetBrains.Annotations;
using NUnit.Framework;
using System.Collections.Generic;
using System.IO;
using Unity.Hierarchy;
using UnityEditor;
using UnityEngine;

[ExecuteAlways]
public class ConvertCSV : MonoBehaviour
{
    public string saveName;
    public string filePath;

    public GameObject stage;

    public string playerName;
    public List<string> enemyNames;

    int count = 0;
    const string ENCODE_TEXT = "Shift_JIS";

    GameObject player = null;
    List<GameObject> enemies = new List<GameObject>();

    public GameObject k;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        Convert();
    }

    // Update is called once per frame
    void Update()
    {
        if (!Application.isPlaying)
        {
            if (Input.GetKeyDown(KeyCode.O))
            {
                //Convert();
            }
            
        }
    }
    bool IsFileExist()
    {
        return File.Exists(filePath + "/" + saveName + ".csv");
    }
    void WriteDate(StreamWriter fs, GameObject obj)
    {
        if (!WriteStageDate(fs, obj))
        {
            if (!WriteCharaDate(fs, obj))
            {

            }
        }
    }
    void WriteTransform(StreamWriter fs, GameObject obj)
    {
        fs.Write(obj.transform.position.x + "_" + obj.transform.position.y + "_" + obj.transform.position.z);
        fs.Write(",");

        fs.Write(obj.transform.localScale.x + "_" + obj.transform.localScale.y + "_" + obj.transform.localScale.z);
        fs.Write(",");

        fs.Write(obj.transform.rotation.eulerAngles.x + "_" + obj.transform.rotation.eulerAngles.y + "_" + obj.transform.rotation.eulerAngles.z);
    }
    void WriteMaterial(StreamWriter fs, GameObject obj)
    {
        var material = obj.GetComponent<MeshRenderer>().material;
        fs.Write(material.color.r + "_" + material.color.g + "_" + material.color.b + "_" + material.color.a);
    }
    void WriteEnemy(StreamWriter fs)
    {
        foreach (var enemy in enemies)
        {
            var comp = enemy.GetComponent<CharacterDate>();
            if (comp == null) return;
            string name = comp.className;
            float hp = comp.hp;

            fs.Write(name);
            fs.Write(",");
            WriteTransform(fs, enemy);
            fs.Write(",");
            WriteMaterial(fs, enemy);
            fs.Write(",");
            fs.Write(hp);
            fs.Write("\n");
            count++;
        }
    }
    bool FindEnemy(string name)
    {
        foreach (var enemyName in enemyNames)
        {
            if (name == enemyName)
            {
                return true;
            }
        }
        return false;
    }
    bool WriteCharaDate(StreamWriter fs, GameObject obj)
    {
        var comp = obj.GetComponent<CharacterDate>();
        if (comp == null) return false;
        string name = comp.className;
        if(name != playerName)
        {
            if (FindEnemy(name))
            {
                enemies.Add(obj);
                return true;
            }
        }
        else
        {
            player = obj;
        }
        float hp = comp.hp;

        fs.Write(name);
        fs.Write(",");
        WriteTransform(fs, obj);
        fs.Write(",");
        WriteMaterial(fs, obj);
        fs.Write(",");
        fs.Write(hp);
        fs.Write("\n");
        count++;
        return true;
    }
    bool WriteStageDate(StreamWriter fs,GameObject obj)
    {
        var nameComp = obj.GetComponent<ClassName>();
        if (nameComp == null) return false;

        string name = nameComp.className;
        
        fs.Write(name);
        fs.Write(",");
        WriteTransform(fs, obj);
        fs.Write(",");
        WriteMaterial(fs, obj);
        fs.Write("\n");
        count++;
        return true;
    }
    void Convert()
    {
        count = 0;
        List<GameObject> objs = new List<GameObject>();
        for (int i = 0; i < stage.transform.childCount; i++)
        {
            objs.Add(stage.transform.GetChild(i).gameObject);
        }
        
        using (var fs = new StreamWriter(filePath + "/" + saveName + ".csv", false, System.Text.Encoding.GetEncoding(ENCODE_TEXT)))
        {
            foreach (var obj in objs)
            {
                WriteDate(fs,obj);
                if(obj.transform.childCount > 0)
                {
                    for (int i = 0; i < obj.transform.childCount; i++)
                    {
                        WriteDate(fs, obj.transform.GetChild(i).gameObject);
                    }
                }
            }
            WriteEnemy(fs);
        }
        if(player == null)
        {
            Debug.LogError("playerが生成されませんでした");
        }
        Debug.Log("ファイル名 : " + saveName + ".csv");
        Debug.Log("エンコード : " + ENCODE_TEXT);
        Debug.Log("書き込んだオブジェクトの数 : " + count);
        Debug.Log("書き込みが完了しました");
    }


    private void OnRenderObject()
    {
        //EditorApplication.QueuePlayerLoopUpdate();
        SceneView.RepaintAll();
    }
}
