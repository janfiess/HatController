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
	// public InputField input_brokerURL;
	public int m_brokerPort = 1883;

	public Text text_deviceId;
	public Text text_setIdMsg;

	ArrayList receiveMessageQueue; // storing MQTT messages so that they can be attached to UI elements
    ArrayList receiveMessageQueue2; 

	private void Awake()
	{
		mqttBehaviour = new MQTTBehaviour();
		// mqttBehaviour.m_brokerURL = m_brokerURL;
		// mqttBehaviour.m_brokerURL = (input_brokerURL.text == "") ? input_brokerURL.placeholder.GetComponent<Text>().text : input_brokerURL.text;	
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
		receiveMessageQueue2 = new ArrayList (); // storing MQTT messages so that they can be attached to UI elements
		
		// string brokerURL = (input_brokerURL.text == "") ? input_brokerURL.placeholder.GetComponent<Text>().text : input_brokerURL.text;	
		// MQTTBehaviour.Connect ();

		// if (MQTTBehaviour.Client != null) {	
		// 	MQTTBehaviour.Client.MqttMsgPublishReceived += OnNewMqttMessage;
		// }

		// // define which topics yu want to subscribe to
		// MQTTBehaviour.s_client.Subscribe (new string[] { "hat/msg" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic1
		// MQTTBehaviour.s_client.Subscribe (new string[] { "hat/getId" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic1
		
		
		// StartMQTT();
	}

	public void StartMQTT(){
		MQTTBehaviour.Connect ();

		if (MQTTBehaviour.Client != null) {	
			MQTTBehaviour.Client.MqttMsgPublishReceived += OnNewMqttMessage;
		}

		// define which topics yu want to subscribe to
		MQTTBehaviour.s_client.Subscribe (new string[] { "hat/msg" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic1
		MQTTBehaviour.s_client.Subscribe (new string[] { "hat/getId" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic1

	}

	void Update(){
		if (receiveMessageQueue.Count >= 1) {
			string queue_input = (string)receiveMessageQueue [0];
            receiveMessageQueue.RemoveRange (0, 1);
			StartCoroutine(GetIdMessage(queue_input));
		}

		if (receiveMessageQueue2.Count >= 1) {
			string queue_input = (string)receiveMessageQueue2 [0];
            receiveMessageQueue2.RemoveRange (0, 1);
			StartCoroutine(SetIdMessage(queue_input));
			// text_setIdMsg.text = queue_input;
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
			receiveMessageQueue.Add (message);
		} 

		if (topic.Contains ("msg")) {
			receiveMessageQueue2.Add (message);
		} 
	}

	IEnumerator GetIdMessage(string receivedText){
		text_deviceId.text = receivedText;
		yield return new WaitForSeconds(6);
		text_deviceId.text = "-";
		yield return null;
	}	

	IEnumerator SetIdMessage(string receivedText){
		text_setIdMsg.text = receivedText;
		yield return new WaitForSeconds(3);
		text_setIdMsg.text = "";
		yield return null;
	}	
}

