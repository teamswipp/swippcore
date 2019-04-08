import React from "react"
import ReactDOM from "react-dom"
import File from "common/file"

ReactDOM.render(
  <img src={File.get("logo.png")}></img>,
  document.getElementById("app")
);

