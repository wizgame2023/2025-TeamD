using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using static Unity.Burst.Intrinsics.X86.Avx;

public class ConvertCSV : MonoBehaviour
{
    public enum Difficulty
    {
        easy,
        normal,
        hard
    }
    public string saveName;
    public string filePath;

    public GameObject stage;

    public string playerName;
    public List<string> enemyNames;
    public Difficulty difficulty;
    int count = 0;
    const string ENCODE_TEXT = "Shift_JIS";
    readonly List<string> LOAD_STAGE = new List<string> { "name", "difficulty","enemys","type" };

    GameObject player = null;

    public Mesh defaultMesh;
    public Material defaultMaterial;
    public void WriteLoadType(StreamWriter fs, List<string> loadType)
    {
        foreach (var type in loadType)
        {
            fs.Write(type);
            fs.Write("_");
        }
        fs.Write("\n");
    }
    public void Convert()
    {
        count = 0;
        player = null;

        List<GameObject> objs = new List<GameObject>();
        for (int i = 0; i < stage.transform.childCount; i++)
        {
            objs.Add(stage.transform.GetChild(i).gameObject);
        }
        
        using (var fs = new StreamWriter(filePath + "/" + saveName + ".csv", false, System.Text.Encoding.GetEncoding(ENCODE_TEXT)))
        {
            fs.Write("stage,");
            fs.Write(difficulty.ToString() + ",");
            for(int i = 0;i < enemyNames.Count; i++)
            {
                fs.Write(enemyNames[i]);
                if(i < enemyNames.Count - 1)
                {
                    fs.Write("_");
                }
            }
            fs.Write(",");
            fs.Write("Stage,");
            WriteLoadType(fs, LOAD_STAGE);
            foreach (var obj in objs)
            {
                var load = obj.GetComponent<LoadDate>();
                if (load)
                {
                    load.Write(fs);
                    count++;
                }
                else
                {
                    if (obj.transform.childCount > 0)
                    {
                        for (int i = 0; i < obj.transform.childCount; i++)
                        {
                            GameObject child = obj.transform.GetChild(i).gameObject;
                            var childLoad = child.GetComponent<LoadDate>();
                            if (!childLoad) continue;

                            childLoad.Write(fs);
                            count++;
                        }
                    }
                }
               
            }
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
        player = null;

        List<string> line = new List<string>();
        using (StreamReader fs = new StreamReader(filePath + "/" + saveName + ".csv", System.Text.Encoding.GetEncoding(ENCODE_TEXT)))
        {
            while(fs.Peek() != -1)
            {
                line.Add(fs.ReadLine());
            }
            DestroyObject();

            List<Wave> waves = new List<Wave>();
            foreach (var str in line)
            {
                string[] date = str.Split(',');
                string[] dateNames = date[date.Length - 1].Split("_");
                string name = "", tag = "", type = "";
                string[] positionStr = {},scaleStr = {},rotationStr = {};
                int index = -1;
                if (FindTag(dateNames, "name",out index) != -1)
                {
                    name = date[index];
                }
                if (FindTag(dateNames, "position", out index) != -1)
                {
                    positionStr = date[index].Split("_");
                }
                if (FindTag(dateNames, "scale", out index) != -1)
                {
                    scaleStr = date[index].Split("_");
                }
                if (FindTag(dateNames, "rotation", out index) != -1)
                {
                    rotationStr = date[index].Split("_");
                }
                if (FindTag(dateNames, "tag", out index) != -1)
                {
                    tag = date[index];
                }
                if (FindTag(dateNames, "type", out index) != -1)
                {
                    type = date[index];
                }
                Vector3 position = Vector3.zero, scale = Vector3.one, rotation = Vector3.zero;
                if(positionStr.Length != 0)
                {
                    position = new Vector3(float.Parse(positionStr[0]), float.Parse(positionStr[1]), float.Parse(positionStr[2]));
                }
                if (scaleStr.Length != 0)
                {
                    scale = new Vector3(float.Parse(scaleStr[0]), float.Parse(scaleStr[1]), float.Parse(scaleStr[2]));
                }
                if (rotationStr.Length != 0)
                {
                    rotation = new Vector3(float.Parse(rotationStr[0]), float.Parse(rotationStr[1]), float.Parse(rotationStr[2]));
                }
                if (type != "Stage")
                {
                    var obj = new GameObject(name);
                    if (tag != "")
                    {
                        obj.transform.tag = tag;
                    }

                    obj.transform.position = position;
                    obj.transform.localScale = scale;
                    obj.transform.eulerAngles = rotation;

                    if (type != "Player" && type != "Enemy")
                    {
                        var filter = obj.AddComponent<MeshFilter>();
                        filter.mesh = defaultMesh;
                        var renderer = obj.AddComponent<MeshRenderer>();
                        renderer.material = defaultMaterial;
                    }
                    LoadDate load = null;
                    if (type == "Wave")
                    {
                        load = obj.AddComponent<Wave>();
                        waves.Add(load as Wave);
                    }
                    else if (type == "Enemy")
                    {
                        load = obj.AddComponent<Enemy>();
                    }
                    else if (type == "Boss")
                    {
                        load = obj.AddComponent<Boss>();
                    }
                    else if (type == "Player")
                    {
                        load = obj.AddComponent<Player>();
                    }
                    else if (type == "Object")
                    {
                        load = obj.AddComponent<LoadDate>();
                    }

                    if (load)
                    {
                        load.Load(date);
                    }

                    obj.transform.parent = stage.transform;
                    if (type == "Enemy")
                    {
                        var enemy = load as Enemy;

                        foreach (var wave in waves)
                        {
                            if (wave.wave == enemy.wave)
                            {
                                obj.transform.parent = wave.gameObject.transform;
                                break;
                            }
                        }
                    }
                    count++;
                }
                else
                {
                    string difficultyStr = "";
                    string[] enemysStr = { };
                    if (FindTag(dateNames, "difficulty", out index) != -1)
                    {
                        difficultyStr = date[index];
                    }
                    if (FindTag(dateNames, "enemys", out index) != -1)
                    {
                        enemysStr = date[index].Split("_");
                    }

                    enemyNames = enemysStr.ToList<String>();
                    
                }
            }
        }

        Debug.Log("読み込んだオブジェクト数 : " + count);
        Debug.Log("ファイル名 : " + saveName + ".csv");
        Debug.Log("読み込みが完了しました");

        EditorApplication.delayCall -= InputCSV;
    }
    int FindTag(string[] date,string tag,out int index)
    {
        index = Array.IndexOf(date, tag);
        return index;
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
