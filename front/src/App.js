import React, { useState } from "react";
import { HashRouter, Routes, Route } from "react-router-dom";
import axios from "axios";

import TopPanel from "./TopPanel";
import WarehouseList from "./WarehouseList";
import Login from "./login/Login";
import Warehouse from "./Warehouse";
import Product from "./Product";
import InvoiceMaster from "./master_menus/Invoice";

import styles from "./App.module.css";

const App = () => {
  const [nextRedirect, setNextRedirect] = useState("/");
  const [viewName, setViewName] = useState("");
  const [currentWarehouse, setCurrentWarehouse] = useState(0);
  const [currentProduct, setCurrentProduct] = useState(0);
  const [masterMenu, setMasterMenu] = useState("");

  const token = localStorage.getItem("token");
  const [user, setUser] = useState({ token: token, user_id: null });

  if (token && !user?.user_id) {
    axios({
      url: "/api/currentuser",
      method: "GET",
      headers: {
        authorization: token
      },
    })
      .then((response) => {
        const user = response.data.result;
        setUser({ ...user, token });
      })
      .catch((error) => {
        console.error(error);
      });
  }

  const switchMasterMenu = () => {
    switch (masterMenu) {
      case "invoice": return <InvoiceMaster user={user} closeMaster={() => setMasterMenu("")} />;
      default: return null;
    }
  }


  return (
    <HashRouter>
      <TopPanel viewName={viewName} setUser={setUser} user={user} setMasterMenu={setMasterMenu} />
      <div className={styles.mainContent}>
        <Routes>
          {["/", "warehouses"].map(path => <Route key={path} path={path} element={<WarehouseList user={user} setNextRedirect={setNextRedirect} setViewName={setViewName} setCurrentWarehouse={setCurrentWarehouse} />} />)}
          <Route path="login" element={<Login setUser={setUser} nextRedirect={nextRedirect} setViewName={setViewName} />} />
          <Route path="warehouse" element={<Warehouse user={user} setNextRedirect={setNextRedirect} setViewName={setViewName} currentWarehouse={currentWarehouse} setCurrentProduct={setCurrentProduct} />} />
          <Route path="product" element={<Product user={user} setNextRedirect={setNextRedirect} setViewName={setViewName} currentProduct={currentProduct} currentWarehouse={currentWarehouse} />} />
        </Routes>
      </div>
      {masterMenu !== "" ?
        <div className={styles.masterMenuWindow}>
          {switchMasterMenu(masterMenu, setMasterMenu)}
          <div className={styles.masterMenuCloseButton} onClick={() => { setMasterMenu("") }}>close</div>
        </div> :
        null}
    </HashRouter>
  );
}

export default App;