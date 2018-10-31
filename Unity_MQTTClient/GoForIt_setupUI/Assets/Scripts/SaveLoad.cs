// attached to Manager Game object

using UnityEngine;
using UnityEngine.UI;
using System;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Collections.Generic;

public class SaveLoad : MonoBehaviour {

	Save saveData;

	public InputField[] input_hat_ids;
	public InputField[] input_brokerInfo;



	void Start () {
		saveData = new Save();
		// Set onChange listeners to most textfields

		for(int i = 0; i < saveData.hat_ids.Length; i++){
			int j = i;
			input_hat_ids[i].onValueChanged.AddListener(delegate {Input_hat_ids_ChangeCheck(j, input_hat_ids[j].text); });
		}

		for(int i = 0; i < saveData.brokerInfo.Length; i++){
			int j = i;
			input_brokerInfo[i].onValueChanged.AddListener(delegate {Input_brokerInfo_ChangeCheck(j, input_brokerInfo[j].text); });
		}
		
		LoadData();	
	}

	public void Input_hat_ids_ChangeCheck(int i, string newText)
    {
		saveData.hat_ids[i] = newText;
		//Convert to Json
		string jsonData = JsonUtility.ToJson(saveData);
		//Save Json string
		PlayerPrefs.SetString("MySettings", jsonData);
		PlayerPrefs.Save();
    }

	public void Input_brokerInfo_ChangeCheck(int i, string newText)
    {
		saveData.brokerInfo[i] = newText;
		//Convert to Json
		string jsonData = JsonUtility.ToJson(saveData);
		//Save Json string
		PlayerPrefs.SetString("MySettings", jsonData);
		PlayerPrefs.Save();
    }



	void LoadData(){
		// if no settings have changed, yet, there is no persistent JSON to load, yet.
		if (!PlayerPrefs.HasKey("MySettings")) return;
		//Load saved Json
		string persistentData = PlayerPrefs.GetString("MySettings");
		//Convert to Class
		Save jsonData = JsonUtility.FromJson<Save>(persistentData);

		// fill Inputfields
		for(int i = 0; i < jsonData.hat_ids.Length; i++){
			input_hat_ids[i].text = jsonData.hat_ids[i];
		}

		for(int i = 0; i < jsonData.brokerInfo.Length; i++){
			input_brokerInfo[i].text = jsonData.brokerInfo[i];
		}
	}
}



[Serializable]
public class Save
{
	public String[] brokerInfo = new String[3];	
	public String[] hat_ids = new String[16];
}