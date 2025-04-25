using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor.Rendering;
using UnityEngine;

public class LoadDate : MonoBehaviour
{
    public string loadName;

    public readonly List<string> LOAD_OBJECT = new List<string> { "name", "position", "scale", "rotation", "tag", "type" };

    public void WriteDefaultInfo(StreamWriter fs)
    {
        fs.Write(transform.position.x + "_" + transform.position.y + "_" + transform.position.z);
        fs.Write(",");

        fs.Write(transform.localScale.x + "_" + transform.localScale.y + "_" + transform.localScale.z);
        fs.Write(",");

        fs.Write(transform.rotation.eulerAngles.x + "_" + transform.rotation.eulerAngles.y + "_" + transform.rotation.eulerAngles.z);
        fs.Write(",");

        fs.Write(transform.tag);
    }
    public virtual void Write(StreamWriter fs)
    {
        fs.Write(loadName + ",");
        WriteDefaultInfo(fs);
        fs.Write(",");
        fs.Write("Object");
        fs.Write(",");
        WriteLoadType(fs, LOAD_OBJECT);
    }

    public void WriteLoadType(StreamWriter fs,List<string> loadType)
    {
        foreach (var type in loadType)
        {
            fs.Write(type);
            fs.Write("_");
        }
        fs.Write("\n");
    }

    public int FindTag(string[] date, string tag, out int index)
    {
        index = Array.IndexOf(date, tag);
        return index;
    }
    public virtual void Load(string[] date)
    {
        loadName = gameObject.name;
    }
}
