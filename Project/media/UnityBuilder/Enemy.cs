using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class Enemy : LoadDate
{
    public int hp;
    [HideInInspector]
    public int wave;

    public readonly List<string> LOAD_ENEMY = new List<string> { "name", "position", "scale", "rotation", "tag", "wave", "hp", "type" };
    public void Write(StreamWriter fs,int wave)
    {
        fs.Write(loadName + ",");
        WriteDefaultInfo(fs);
        fs.Write(",");
        fs.Write(wave + ",");
        fs.Write(hp + ",");
        fs.Write("Enemy");
        fs.Write(",");
        WriteLoadType(fs, LOAD_ENEMY);
    }

    public override void Write(StreamWriter fs)
    {
        
    }
    public override void Load(string[] date)
    {
        int index = -1;
        loadName = gameObject.name;
        string[] indexDate = date[date.Length - 1].Split("_");

        if (FindTag(indexDate, "hp", out index) != -1)
        {
            hp = int.Parse(date[index]);
        }
        if (FindTag(indexDate, "wave", out index) != -1)
        {
            wave = int.Parse(date[index]);
        }
    }
}
