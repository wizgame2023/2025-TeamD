using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class Wave : LoadDate
{
    public int wave;
    public int maxEnemyCount;
    public int popInterval;

    readonly List<string> LOAD_WAVE = new List<string> { "name", "wave", "max", "interval", "type" };
    public override void Write(StreamWriter fs)
    {
        fs.Write(loadName + ",");
        fs.Write(wave + ",");
        fs.Write(maxEnemyCount + ",");
        fs.Write(popInterval + ",");
        fs.Write("Wave");
        fs.Write(",");
        WriteLoadType(fs, LOAD_WAVE);

        for(int i = 0;i < transform.childCount; i++)
        {
            GameObject child = transform.GetChild(i).gameObject;
            Enemy enemy = child.GetComponent<Enemy>();
            if (enemy)
            {
                enemy.Write(fs, wave);
            }
        }
    }

    public override void Load(string[] date)
    {
        int index = -1;
        loadName = gameObject.name;
        string[] indexDate = date[date.Length - 1].Split("_");
        if (FindTag(indexDate, "wave",out index) != -1)
        {
            wave = 1;
        }
        if (FindTag(indexDate, "max", out index) != -1)
        {
            maxEnemyCount = int.Parse(date[index]);
        }
        if (FindTag(indexDate, "interval", out index) != -1)
        {
            popInterval = int.Parse(date[index]);
        }

        gameObject.name += wave;
    }
}
