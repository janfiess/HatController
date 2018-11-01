﻿/*********************************************************************
 * GO FOR IT - Swarm hats setup tool
 * Trigger lights on hat using MQTT
 * mail@janfiess.com, fall 2018
 *
 * This script is attached to the Manager GameObject
 * Publishing MQTT messages to the MQTT broker when UI buttons pressed
 * MQTTBehaviour is MQTT backend, also attached to Manager GameObject
 *********************************************************************/

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Trigger_hat : MonoBehaviour {
	public InputField input_ssid, input_pw, input_brokerSettings;
	MQTTBehaviour mqttBehaviour;
	void Awake(){
		mqttBehaviour = new MQTTBehaviour();
	}

	/*
	 * when any anim button (eg. "HAT 1") is pressed
	 */
	
	// BLACK constant solid (called via buttons)
	public void OnAnim_1Hat_black(InputField input_deviceId){
		string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
		MQTTBehaviour.Publish ("janfiess/" + deviceId, "a1");
	}

	// WHITE constant solid (called via buttons)
	public void OnAnim_1Hat_white(InputField input_deviceId){
		string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
		MQTTBehaviour.Publish ("janfiess/" + deviceId, "a2");
	}

	// Running Light WHITE (called via buttons)
	public void OnAnim_1Hat_runningWhite(InputField input_deviceId){
		string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
		MQTTBehaviour.Publish ("janfiess/" + deviceId, "a3");
	}

	// Pulsing Light WHITE (called via buttons)
	public void OnAnim_1Hat_pulsingWhite(InputField input_deviceId){
		string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
		MQTTBehaviour.Publish ("janfiess/" + deviceId, "a4");
	}

	// ORANGE constant solid (called via buttons)
	public void OnAnim_1Hat_orange(InputField input_deviceId){
		string deviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
		MQTTBehaviour.Publish ("janfiess/" + deviceId, "a5");
	}

	/* 
	 * When the anim button "ALL HATS" is pressed
	 */
	public void OnAnim_AllHats(string msg){
		string mqttAddress ="janfiess/allHats";
		MQTTBehaviour.Publish (mqttAddress, msg);
	}

	// when the button Set ID/"ASSIGN" is pressed
	public void OnSetDeviceId(InputField input_deviceId){
		string newDeviceId = (input_deviceId.text == "") ? input_deviceId.placeholder.GetComponent<Text>().text : input_deviceId.text;
		// print(mqttAddress + "   |   " + newDeviceId);
		MQTTBehaviour.Publish ("janfiess/id", newDeviceId);
	}

	// when the button Set ID/"Set SSID, PW, Broker IP" is pressed
	public void OnSetupBroker(){
		string ssid =           (input_ssid.text == "") ?           input_ssid.placeholder.GetComponent<Text>().text :           input_ssid.text;
		string pw =             (input_pw.text == "") ?             input_pw.placeholder.GetComponent<Text>().text :             input_pw.text;
		string brokerSettings = (input_brokerSettings.text == "") ? input_brokerSettings.placeholder.GetComponent<Text>().text : input_brokerSettings.text;
	
		MQTTBehaviour.Publish ("janfiess/ssid", ssid);
		MQTTBehaviour.Publish ("janfiess/pw", pw);
		MQTTBehaviour.Publish ("janfiess/broker", brokerSettings);
		// print(mqttAddress + "   |   " + ssid + "   |   " + pw + "   |   " + brokerSettings);
	}
}
