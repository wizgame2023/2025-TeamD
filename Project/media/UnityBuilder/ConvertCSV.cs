using JetBrains.Annotations;
using NUnit.Framework;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class ConvertCSV : MonoBehaviour
{
    public string saveName;
    public string filePath;

    public GameObject stage;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        Convert();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    bool IsFileExist()
    {
        return File.Exists(filePath + "/" + saveName + ".csv");
    }

    void Convert()
    {
        List<GameObject> objs = new List<GameObject>();
        for (int i = 0; i < stage.transform.childCount; i++)
        {
            objs.Add(stage.transform.GetChild(i).gameObject);
        }
        
        using (var fs = new StreamWriter(filePath + "/" + saveName + ".csv", false, System.Text.Encoding.GetEncoding("Shift_JIS")))
        {
            foreach (var obj in objs)
            {
                var nameComp = obj.GetComponent<ClassName>();
                string name = "";
                if (nameComp != null)
                {
                    name = nameComp.className;
                }
                fs.Write(name);
                fs.Write(",");

                fs.Write(obj.transform.position.x + "_" + obj.transform.position.y + "_" + obj.transform.position.z);
                fs.Write(",");

                fs.Write(obj.transform.localScale.x + "_" + obj.transform.localScale.y + "_" + obj.transform.localScale.z);
                fs.Write(",");

                fs.Write(obj.transform.rotation.eulerAngles.x + "_" + transform.rotation.eulerAngles.y + "_" + transform.rotation.eulerAngles.z);
                fs.Write("\n");
            }
        }
        
    }
}
