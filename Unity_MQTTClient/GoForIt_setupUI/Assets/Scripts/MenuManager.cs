using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MenuManager : MonoBehaviour {

	public GameObject panel_1, panel_2, panel_3;
	[HideInInspector] public GameObject activePanel;
	[HideInInspector] public List<GameObject> menu_panels;


	void Start () {
		menu_panels.Add(panel_1);
		menu_panels.Add(panel_2);
		menu_panels.Add(panel_3);

		foreach(GameObject menu_panel in menu_panels){
			menu_panel.SetActive(false);
		}

	}
	
	public void ShowPanel (GameObject currentPanel) {
		foreach(GameObject menu_panel in menu_panels){
			menu_panel.SetActive(false);
		}

		currentPanel.SetActive(true);
		activePanel = currentPanel;
	}
}
