/*********************************************************************
 * GO FOR IT - Swarm hats setup tool
 * Trigger lights on hat using MQTT
 * mail@janfiess.com, fall 2018
 *
 * This script is attached to the Manager GameObject
 * Setting up MQTT connection + Receiving MQTT from the MQTT broker
 * MQTTBehaviour is MQTT backend, also attached to Manager GameObject
 *********************************************************************/



using System.Text;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;
using UnityEditor.VersionControl;

public class Mqtt_Client : MonoBehaviour {
	// singleton instance
	private static Mqtt_Client s_instance;
	// unity interface. store instance as singleton reference.
	MQTTBehaviour mqttBehaviour;

	// public string m_brokerURL = "127.0.0.1"; //"test.mosquitto.org";
	public InputField input_brokerURL;
	public int m_brokerPort = 1883;

	public Text text_deviceId;
	static public Text text_deviceId_static;

	ArrayList receiveMessageQueue; // storing MQTT messages so that they can be attached to UI elements
    
	private void Awake()
	{
		mqttBehaviour = new MQTTBehaviour();
		// mqttBehaviour.m_brokerURL = m_brokerURL;
		mqttBehaviour.m_brokerURL = (input_brokerURL.text == "") ? input_brokerURL.placeholder.GetComponent<Text>().text : input_brokerURL.text;	
		mqttBehaviour.m_brokerPort = m_brokerPort;

		if (Mqtt_Client.s_instance == null) {
			Mqtt_Client.s_instance = this;
			DontDestroyOnLoad (this.gameObject);
		} else {
			Destroy (this.gameObject); 
		}
		
	}

	private void Start () {
		receiveMessageQueue = new ArrayList (); // storing MQTT messages so that they can be attached to UI elements
		
		MQTTBehaviour.Connect ();

		if (MQTTBehaviour.Client != null) {	
			MQTTBehaviour.Client.MqttMsgPublishReceived += OnNewMqttMessage;
		}

		// define which topics yu want to subscribe to
		MQTTBehaviour.s_client.Subscribe (new string[] { "janfiess/#" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic1
	}

	void Update(){
		if (receiveMessageQueue.Count >= 1) {
			string queue_input = (string)receiveMessageQueue [0];
            receiveMessageQueue.RemoveRange (0, 1);
			text_deviceId.text = queue_input;
		}
	}

	// receiving messages
	public void OnNewMqttMessage(object sender, MqttMsgPublishEventArgs e)
	{ 
		// handle message received 
		string topic = e.Topic;
		string message = Encoding.UTF8.GetString (e.Message);
		// Debug.Log (e.Message.Length + " bytes recieved on topic " + topic);
		// print("Topic: " + topic + " | Message: " + message);

		if (topic.Contains ("getId")) {
			print(message);
			receiveMessageQueue.Add (message);
		} 
	}	
}