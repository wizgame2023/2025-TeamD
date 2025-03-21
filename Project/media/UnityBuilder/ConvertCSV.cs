using JetBrains.Annotations;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

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

    public Mesh defaultMesh;
    public Material defaultMaterial;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
      
        //Convert();
    }

    // Update is called once per frame
    void Update()
    {
      
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
    void WriteLoadDate(StreamWriter fs, GameObject obj,bool isChara)
    {
        if (isChara)
        {
            fs.Write("Chara");
        }
        else
        {
            fs.Write("Object");
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
            fs.Write(hp);
            fs.Write(",");
            WriteLoadDate(fs, enemy,true);
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
        fs.Write(hp);
        fs.Write(",");
        WriteLoadDate(fs, obj, true);
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
        WriteLoadDate(fs, obj, false);
        fs.Write("\n");
        count++;
        return true;
    }
    public void Convert()
    {
        count = 0;
        enemies.Clear();
        player = null;

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

    public void InputCSV()
    {
        List<string> line = new List<string>();
        using (StreamReader fs = new StreamReader(filePath + "/" + saveName + ".csv", System.Text.Encoding.GetEncoding(ENCODE_TEXT)))
        {
            while(fs.Peek() != -1)
            {
                line.Add(fs.ReadLine());
            }
            DestroyObject();

            foreach (var str in line)
            {
                string[] date = str.Split(',');

                string name = date[0];
                string[] positionStr = date[1].Split("_");
                string[] scaleStr = date[2].Split("_");
                string[] rotationStr = date[3].Split("_");
                string[] InfoStr = date[date.Length - 1].Split("_");

                Vector3 position = new Vector3(float.Parse(positionStr[0]), float.Parse(positionStr[1]), float.Parse(positionStr[2]));
                Vector3 scale = new Vector3(float.Parse(scaleStr[0]), float.Parse(scaleStr[1]), float.Parse(scaleStr[2]));
                Vector3 rotation = new Vector3(float.Parse(rotationStr[0]), float.Parse(rotationStr[1]), float.Parse(rotationStr[2]));

                var obj = new GameObject(name);
                var filter = obj.AddComponent<MeshFilter>();
                filter.mesh = defaultMesh;
                var renderer = obj.AddComponent<MeshRenderer>();
                renderer.material = defaultMaterial;
                if (InfoStr[0] == "Chara")
                {
                    var comp = obj.AddComponent<CharacterDate>();
                    comp.className = name;
                }
                else
                {
                    var comp = obj.AddComponent<ClassName>();
                    comp.className = name;
                }
                obj.transform.parent = stage.transform;

                obj.transform.position = position;
                obj.transform.localScale = scale;
                obj.transform.eulerAngles = rotation;
            }
            
        }

        EditorApplication.delayCall -= InputCSV;
    }

    public void DestroyObject()
    {
        List<GameObject> objs = new List<GameObject>();
        for (int i = 0; i < stage.transform.childCount; i++)
        {
            objs.Add(stage.transform.GetChild(i).gameObject);
        }
        foreach (var obj in objs)
        {
            DestroyImmediate(obj);
        }
        //var charaDates = FindObjectsByType<CharacterDate>(FindObjectsSortMode.InstanceID);
        //foreach (var obj in charaDates)
        //{
        //    DestroyImmediate(obj);
        //}
    }
}
