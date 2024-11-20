import React, { Component } from "react";
import { Route, NavLink, Routes, HashRouter } from "react-router-dom";

import Home from "./Home";
import About from "./About";
import Contact from "./Contact";

class App extends Component {
  render() {
    return (
      <HashRouter>
        <div className="App">
          <h1>A Simple SPA made using React</h1>
          <ul className="header">
            <li><NavLink to="/">Home</NavLink></li>
            <li><NavLink to="/about">About</NavLink></li>
            <li><NavLink to="/contact">Contact</NavLink></li>
          </ul>
          <div className="content">
            <Routes>
              <Route exact path="/" element={<Home />}></Route>
              <Route exact path="/about" element={<About />}></Route>
              <Route exact path="/contact" element={<Contact />}></Route>
            </Routes>
          </div>
        </div>
      </HashRouter>
    );
  }
}
export default App;