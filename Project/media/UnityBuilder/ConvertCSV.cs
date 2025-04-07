using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;
using static Unity.Burst.Intrinsics.X86.Avx;

public class ConvertCSV : MonoBehaviour
{
    
    public string saveName;
    public string filePath;

    public GameObject stage;

    public string playerName;
    public List<string> enemyNames;

    int count = 0;
    const string ENCODE_TEXT = "Shift_JIS";
    readonly List<string> LOAD_OBJECT = new List<string> { "name", "position", "scale", "rotation","tag","collision","type" };
    readonly List<string> LOAD_CHARA = new List<string> { "name", "position", "scale", "rotation", "tag", "hp","region", "type" };
    readonly List<string> LOAD_CHARA_CONDITION = new List<string> { "name", "position", "scale", "rotation", "tag","hp","region","time","defeat", "type" };
    readonly List<string> LOAD_POINTER = new List<string> { "name", "position", "scale", "rotation", "tag","number","connect","type" };


    GameObject player = null;
    List<GameObject> enemies = new List<GameObject>();
    List<GameObject> pointers = new List<GameObject>();
    int pointerCount = 0;
    public Mesh defaultMesh;
    public Material defaultMaterial;
    string GetObjectType(GameObject obj)
    {
        if (obj.GetComponent<ClassName>() != null)
        {
            return "Object";
        }
        else if (obj.GetComponent<CharacterDate>() != null)
        {
            return "Chara";
        }
        else if (obj.GetComponent<RootPointer>() != null)
        {
            return "Pointer";
        }
        return "";
    }
    void WriteDate(StreamWriter fs, GameObject obj)
    {
        string type = GetObjectType(obj);
        switch (type)
        {
            case "Object":
                WriteStageDate(fs, obj);
                break;
            case "Chara":
                WriteCharaDate(fs, obj);
                break;
            case "Pointer":
                GetPointer(fs, obj);
                break;
        }
    }
    void WriteDateType(StreamWriter fs, List<string> loadType)
    {
        foreach (var type in loadType)
        {
            fs.Write(type);
            fs.Write("_");
        }
    }
    bool GetPointer(StreamWriter fs,GameObject obj)
    {
        var comp = obj.GetComponent<RootPointer>();
        if (comp == null) return false;
        comp.number = pointerCount;
        pointerCount++;
        pointers.Add(obj);
        return true;
    }
    void WritePointer(StreamWriter fs)
    {
        foreach (var obj in pointers)
        {
            var comp = obj.GetComponent<RootPointer>();
            if (comp == null) continue;
            string name = comp.className;
            int number = comp.number;
            var movePointer = comp.pointer;

            fs.Write(name);
            fs.Write(",");
            WriteDefaultInfo(fs, obj);
            fs.Write(",");
            fs.Write(number);
            fs.Write(",");
            foreach(var movePoint in movePointer)
            {
                fs.Write(movePoint.GetComponent<RootPointer>().number);
                fs.Write("_");
            }
            fs.Write(",");
            fs.Write(GetObjectType(obj));
            fs.Write(",");
            WriteDateType(fs, LOAD_POINTER);
            fs.Write("\n");
        }
    }
    void WriteDefaultInfo(StreamWriter fs, GameObject obj)
    {
        fs.Write(obj.transform.position.x + "_" + obj.transform.position.y + "_" + obj.transform.position.z);
        fs.Write(",");

        fs.Write(obj.transform.localScale.x + "_" + obj.transform.localScale.y + "_" + obj.transform.localScale.z);
        fs.Write(",");

        fs.Write(obj.transform.rotation.eulerAngles.x + "_" + obj.transform.rotation.eulerAngles.y + "_" + obj.transform.rotation.eulerAngles.z);
        fs.Write(",");

        fs.Write(obj.transform.tag);
    }
    void WriteEnemy(StreamWriter fs)
    {
        foreach (var enemy in enemies)
        {
            WriteCharacterDate(fs, enemy);
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
    bool CheckEnemy(GameObject obj)
    {
        var comp = obj.GetComponent<CharacterDate>();
        if (comp == null) return false;
        string name = comp.className;
        if (name != playerName)
        {
            if (FindEnemy(name))
            {
                enemies.Add(obj);
                return true;
            }
        }
        return false;
    }
    bool WriteCharaDate(StreamWriter fs, GameObject obj)
    {
        var comp = obj.GetComponent<CharacterDate>();
        if (comp == null) return false;
        if(!CheckEnemy(obj))
        {
            player = obj;
        }
        else
        {
            return true;
        }
        WriteCharacterDate(fs, obj);
        count++;
        return true;
    }
    void WriteCharacterDate(StreamWriter fs, GameObject obj)
    {
        var comp = obj.GetComponent<CharacterDate>();
        if (comp == null) return;
        string name = comp.className;
        float hp = comp.hp;
        int region = comp.region;

        fs.Write(name);
        fs.Write(",");
        WriteDefaultInfo(fs, obj);
        fs.Write(",");
        fs.Write(hp);
        fs.Write(",");
        fs.Write(region);
        fs.Write(",");
        if (comp.isConditional)
        {
            fs.Write(comp.time);
            fs.Write(",");
            fs.Write(comp.defeatEnemyCount);
            fs.Write(",");
        }
        fs.Write(GetObjectType(obj));
        fs.Write(",");
        if(comp.isConditional)
        {
            WriteDateType(fs, LOAD_CHARA_CONDITION);
        }
        else
        {
            WriteDateType(fs, LOAD_CHARA);
        }
        fs.Write("\n");
    }
    bool WriteStageDate(StreamWriter fs,GameObject obj)
    {
        var nameComp = obj.GetComponent<ClassName>();
        if (nameComp == null) return false;

        string name = nameComp.className;
        var collisionComp = obj.GetComponent<Collider>();
        fs.Write(name);
        fs.Write(",");
        WriteDefaultInfo(fs, obj);
        fs.Write(",");
        if (collisionComp == null)
        {
            fs.Write("true");
        }
        else
        {
            fs.Write("false");
        }
        fs.Write(",");
        fs.Write(GetObjectType(obj));
        fs.Write(",");
        WriteDateType(fs, LOAD_OBJECT);
        fs.Write("\n");
        count++;
        return true;
    }
    public void Convert()
    {
        count = 0;
        pointerCount = 0;
        enemies.Clear();
        pointers.Clear();
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
            WritePointer(fs);
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
        count = 0;
        pointerCount = 0;
        enemies.Clear();
        pointers.Clear();
        player = null;

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
                string[] dateNames = date[date.Length - 1].Split("_");

                string name = date[Array.IndexOf(dateNames,"name")];
                string[] positionStr = date[Array.IndexOf(dateNames, "position")].Split("_");
                string[] scaleStr = date[Array.IndexOf(dateNames, "scale")].Split("_");
                string[] rotationStr = date[Array.IndexOf(dateNames, "rotation")].Split("_");
                string tag = date[Array.IndexOf(dateNames, "tag")];
                string type = date[Array.IndexOf(dateNames, "type")];

                Vector3 position = new Vector3(float.Parse(positionStr[0]), float.Parse(positionStr[1]), float.Parse(positionStr[2]));
                Vector3 scale = new Vector3(float.Parse(scaleStr[0]), float.Parse(scaleStr[1]), float.Parse(scaleStr[2]));
                Vector3 rotation = new Vector3(float.Parse(rotationStr[0]), float.Parse(rotationStr[1]), float.Parse(rotationStr[2]));

                var obj = new GameObject(name);
                obj.transform.tag = tag;
                var filter = obj.AddComponent<MeshFilter>();
                filter.mesh = defaultMesh;
                var renderer = obj.AddComponent<MeshRenderer>();
                renderer.material = defaultMaterial;
                if (type == "Chara")
                {
                    var comp = obj.AddComponent<CharacterDate>();
                    comp.className = name;
                    comp.hp = int.Parse(date[Array.IndexOf(dateNames, "hp")]);
                    comp.region = int.Parse(date[Array.IndexOf(dateNames, "region")]);
                    int index = Array.IndexOf(dateNames, "time");
                    if(index != -1)
                    {
                        comp.isConditional = true;
                        comp.time = int.Parse(date[index]);
                        comp.defeatEnemyCount = int.Parse(date[Array.IndexOf(dateNames, "defeat")]);
                    }
                }
                else if(type == "Object")
                {
                    var comp = obj.AddComponent<ClassName>();
                    comp.className = name;
                }
                else if(type == "Pointer")
                {
                    var comp = obj.AddComponent<RootPointer>();
                    comp.className = name;
                    comp.number = int.Parse(date[Array.IndexOf(dateNames, "number")]);
                    comp.loadPointer = date[Array.IndexOf(dateNames, "connect")];
                    pointers.Add(obj);
                }
                    obj.transform.parent = stage.transform;

                obj.transform.position = position;
                obj.transform.localScale = scale;
                obj.transform.eulerAngles = rotation;

                count++;
            }
            foreach (var obj in pointers)
            {
                RootPointer rootPointer = obj.GetComponent<RootPointer>();
                string[] numbers = rootPointer.loadPointer.Split("_");
                foreach (var point in pointers)
                {
                    foreach(var number in numbers)
                    {
                        if(number == "") continue;
                        RootPointer otherPointer = point.GetComponent<RootPointer>();
                        if(otherPointer.number == int.Parse(number))
                        {
                            rootPointer.pointer.Add(point);
                        }
                    }
                }
            }
            
        }

        Debug.Log("読み込んだオブジェクト数 : " + count);
        Debug.Log("ファイル名 : " + saveName + ".csv");
        Debug.Log("読み込みが完了しました");

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
    }
}
