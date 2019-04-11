import React from "react";
import { ipcRenderer } from "electron";
import { library } from "@fortawesome/fontawesome-svg-core";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faSpinner, faWindowMinimize, faWindowMaximize, faWindowClose } from "@fortawesome/free-solid-svg-icons";
import "./controlbar.css"

library.add(faSpinner, faWindowMinimize, faWindowMaximize, faWindowClose);

export default class ControlBar extends React.Component {
	constructor(props) {
		super(props);
		this.state = { showSpinner: false };

		ipcRenderer.on("state", (event, message) => {
			this.setState({ showSpinner: (message == "working" ? true : false) });
		});
	}

	render() {
		var onMinimzie = function() {
		}

		var onMaximize = function() {
		}

		var onClose = function() {
			ipcRenderer.send("exit");
		}

		return(
			<div className="controlbar">
				<div>
					<img className="piclet"/>
					<div>{this.props.headerText}</div>
				</div>
				<div>
					{this.state.showSpinner == true &&
						<FontAwesomeIcon className="spinner" icon="spinner" spin/>
					}
					{this.props.fullControls == true &&
						<div>
							<FontAwesomeIcon className="minimize" icon="window-minimize"/>
							<FontAwesomeIcon className="maximize" icon="window-maximize"/>
						</div>
					}
					<FontAwesomeIcon onClick={onClose} className="close" icon="window-close"/>
				</div>
			</div>
		);
	}
}

