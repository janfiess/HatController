// MQTTBehaviour.cs: MQTT backend
// nothing to change here
// Subscribe, publish MQTT messages, get MQTT Messages in MQTT_Client.cs


using System.Collections;
using System.Net;
using System.Text;
using System.Collections.Generic;
using UnityEngine;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

public class MQTTBehaviour : MonoBehaviour {
	
	private static MQTTBehaviour s_instance;
	
	private void Awake () {
		MQTTBehaviour.s_instance = this;
	}

	private const string ClientIdPrefkey = "clientId";

	public static MqttClient s_client;
	public static MqttClient Client {
		get {
			return s_client;
		}
	}

	private static string s_clientId;



	[HideInInspector] public string m_brokerURL = "test.mosquitto.org";
	[HideInInspector] public int m_brokerPort = 1883;


	
	public static void Connect () {
		MQTTBehaviour.s_instance.DoConnect ();
	}
	public void DoConnect () {
		print("DoConnect");
		if ((s_client == null) || (!s_client.IsConnected)) {
			s_client = new MqttClient (m_brokerURL, m_brokerPort, false, null, null, MqttSslProtocols.None);

			if (PlayerPrefs.HasKey (ClientIdPrefkey)) {
				s_clientId = PlayerPrefs.GetString (ClientIdPrefkey);
				// Debug.Log ("Reused id : " + s_clientId); 
			} else {
				s_clientId = System.Guid.NewGuid ().ToString ();
				PlayerPrefs.SetString (ClientIdPrefkey, s_clientId);
				// Debug.Log ("New id : " + s_clientId); 
			}
			s_client.Connect (s_clientId);

			
		}
	}

	public static void Publish (string p_topic, string p_txt) {
		MQTTBehaviour.Publish (p_topic, p_txt, MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
	}
	public static void Publish (string p_topic, string p_txt, byte p_qos, bool p_retain) {
		MQTTBehaviour.s_instance.DoPublish (p_topic, p_txt, p_qos, p_retain);
	}
	public void DoPublish (string p_topic, string p_txt, byte p_qos, bool p_retain) {
		byte[] msg = Encoding.UTF8.GetBytes (p_txt);
		s_client.Publish (p_topic, msg, p_qos, p_retain); 
	}
}