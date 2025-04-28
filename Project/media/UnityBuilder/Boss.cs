using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class Boss : LoadDate
{
    public int hp;

    public readonly List<string> LOAD_BOSS = new List<string> { "name", "position", "scale", "rotation", "tag", "hp", "type" };

    public override void Write(StreamWriter fs)
    {
        fs.Write(loadName + ",");
        WriteDefaultInfo(fs);
        fs.Write(",");
        fs.Write(hp + ",");
        fs.Write("Boss");
        fs.Write(",");
        WriteLoadType(fs, LOAD_BOSS);
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
    }
}
