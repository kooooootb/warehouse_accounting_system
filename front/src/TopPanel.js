import React from "react";
import { useNavigate, useLocation } from "react-router-dom";

import styles from "./TopPanel.module.css"

export default function TopPanel({ viewName, setUser, user, setMasterMenu }) {
    const navigate = useNavigate()
    const location = useLocation();

    const signOut = () => {
        setUser({ user_id: null, token: null });
        localStorage.removeItem("token");
        navigate("login");
        setMasterMenu("");
    };

    return (
        <div className={styles.topPanel} >
            <div className={styles.viewName}>
                {viewName}
            </div>
            {location.pathname !== "/login" ? <div className={styles.panelButton} onClick={() => navigate("/")} >
                Home
            </div> : null}
            {location.pathname !== "/login" ? <div className={styles.panelButton} onClick={() => setMasterMenu("invoice")} >
                Create invoice
            </div> : null}
            {location.pathname !== "/login" ? <div className={styles.panelButton} onClick={() => setMasterMenu("create_warehouse")} >
                Create warehouse
            </div> : null}
            {location.pathname !== "/login" ? <div className={styles.panelButton} onClick={() => setMasterMenu("create_user")} >
                Create user
            </div> : null}
            {location.pathname !== "/login" ? <div className={styles.currentUser} >
                {user.name ? <p>Currently signed in as: "{user.name}"</p> : <p>not signed</p>}
            </div> : null}
            {location.pathname !== "/login" ? <div className={styles.panelButton} onClick={signOut} >
                Sign out
            </div> : null}
        </div>
    );
}
