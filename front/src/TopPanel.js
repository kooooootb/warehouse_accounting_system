import React from "react";
import { useNavigate } from "react-router-dom";

import styles from "./TopPanel.module.css"

export default function TopPanel({ viewName, setUser, user }) {
    const navigate = useNavigate()

    const signOut = () => {
        setUser({ user_id: null, token: null });
        localStorage.removeItem("token");
        navigate("login");
    };

    return (
        <div className={styles.topPanel} >
            <div className={styles.viewName}>
                {viewName}
            </div>
            <div className={styles.toWarehouses} onClick={() => navigate("/")} >
                Home
            </div>
            <div className={styles.currentUser} >
                {user.name ? <p>Currently signed in as: "{user.name}"</p> : <p>not signed</p>}
            </div>
            <div className={styles.signOut} onClick={signOut} >
                Sign out
            </div>
        </div>
    );
}
