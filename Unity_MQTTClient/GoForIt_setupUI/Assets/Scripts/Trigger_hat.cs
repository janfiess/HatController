using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Trigger_hat : MonoBehaviour {
	public InputField input_ssid, input_pw, input_brokerSettings;

	// when any anim button (eg. "HAT 1") is pressed
	public void OnAnim_1Hat(InputField input_deviceId){
		string deviceId;
		if(input_deviceId.text == "") deviceId = input_deviceId.placeholder.GetComponent<Text>().text;
		else deviceId = input_deviceId.text;
		print(deviceId);

		string mqttAddress = "janfiess/" + deviceId;
	}

	// when the anim button "ALL HATS" is pressed
	public void OnAnim_AllHats(){
		string mqttAddress ="janfiess/allHats";
		print(mqttAddress);
	}

	// when the button Set ID/"ASSIGN" is pressed
	public void OnSetDeviceId(InputField input_deviceId){
		string mqttAddress ="janfiess/setId";
		string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
	
		print(mqttAddress + "   |   " + deviceId);
	}

	// when the button Set ID/"Set SSID, PW, Broker IP" is pressed
	public void OnSetupBroker(){
		string mqttAddress ="janfiess/broker";

		string ssid =           (input_ssid.text == "") ?           input_ssid.placeholder.GetComponent<Text>().text :           input_ssid.text;
		string pw =             (input_pw.text == "") ?             input_pw.placeholder.GetComponent<Text>().text :             input_pw.text;
		string brokerSettings = (input_brokerSettings.text == "") ? input_brokerSettings.placeholder.GetComponent<Text>().text : input_brokerSettings.text;
	
		print(mqttAddress + "   |   " + ssid + "   |   " + pw + "   |   " + brokerSettings);
	}
}
