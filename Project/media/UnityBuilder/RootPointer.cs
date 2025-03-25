using System.Collections.Generic;
using UnityEngine;

public class RootPointer : MonoBehaviour
{
    public string className; 
    public List<GameObject> pointer = new List<GameObject>();
    public int number;

    [HideInInspector]
    public string loadPointer;
}
