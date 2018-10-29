// publish MQTT messages, get messages -> Subscribe
// MQTTBehaviour is MQTT backend


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

	public string m_brokerURL = "127.0.0.1"; //"test.mosquitto.org";
	public int m_brokerPort = 1883;

    
	private void Awake()
	{
		mqttBehaviour = new MQTTBehaviour();
		mqttBehaviour.m_brokerURL = m_brokerURL;
		mqttBehaviour.m_brokerPort = m_brokerPort;

		if (Mqtt_Client.s_instance == null) {
			Mqtt_Client.s_instance = this;
			DontDestroyOnLoad (this.gameObject);
		} else {
			Destroy (this.gameObject); 
		}
		
	}

	private void Start () {
		

		MQTTBehaviour.Connect ();

		if (MQTTBehaviour.Client != null) {	
			MQTTBehaviour.Client.MqttMsgPublishReceived += OnNewMqttMessage;
		}


		// define which topics yu want to subscribe to
			MQTTBehaviour.s_client.Subscribe (new string[] { "janfiess/#" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic1
			MQTTBehaviour.s_client.Subscribe (new string[] { "janfiess/static" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE }); // subscribes to the topic janfiess/mySubtopic2 

		// publish
		Invoke("PublishMsg",2);
	}

	void PublishMsg()
	{
		string message = "222";
		string subTopic1 = "janfiess/anim";
		MQTTBehaviour.Publish (subTopic1, message);
		print("Message published: " + message);
	}

	// receiving messages
	public static void OnNewMqttMessage(object sender, MqttMsgPublishEventArgs e)
	{ 
		// handle message received 
		string topic = e.Topic;
		string message = Encoding.UTF8.GetString (e.Message);
		//Debug.Log (e.Message.Length + " bytes recieved on topic " + topic);
		print("Topic: " + topic + " | Message: " + message);

		if (topic.Contains ("anim")) {
			print("do something when received topic \"anim\"");
		} 

		if (topic.Contains ("static")) {
			print("do something ELSE when received topic \"static\"");
		} 
	}
}