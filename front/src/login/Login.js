import React from "react";
import axios from "axios";
import { Navigate } from "react-router-dom";

import styles from "./Login.module.css"

export default class Login extends React.Component {
    constructor(props) {
        super(props)
        this.state = {
            user_id: null
        }

        this.props.setViewName("Login");
    }

    handleSubmit = (e) => {
        e.preventDefault();

        const form = e.target;
        const formData = new FormData(form);
        const formJson = Object.fromEntries(formData.entries());

        const illegalChars = /[^a-zA-Z0-9]/;

        if (illegalChars.test(formJson.login)) {
            alert("Login contains illegal characters. Only letters and numbers are allowed.");
            return;
        }

        axios.post("/api/authorization", formJson)
            .then((response) => {
                const user = response.data;
                this.setState(user);
                this.props.setUser(user);
                localStorage.setItem("token", user.token);
            })
            .catch((error) => {
                if (error.status === 401) {
                    alert("Incorrect credentials, access denied")
                }
                console.log(error);
            });
    }


    render() {
        if (this.state.user_id !== null)
            return (
                <Navigate to={this.props.nextRedirect} />
            )
        else
            return (
                <div className={styles.authContainer}>
                    <h1>Login page</h1>
                    <form method="post" onSubmit={this.handleSubmit}>
                        <div className={styles.formGroup}>
                            <label>Login:</label>
                            <input type="text" name="login" placeholder="Enter_your_login" required />
                        </div>
                        <div className={styles.formGroup}>
                            <label>Password:</label>
                            <input type="password" name="password" placeholder="Enter_your_password" required />
                        </div>
                        <div className={styles.submitBtn}>
                            <button type="submit">Submit</button>
                        </div>
                    </form>
                </div>
            )
    }
}