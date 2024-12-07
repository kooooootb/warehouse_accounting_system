import React from "react";
import axios from "axios";

import styles from "./CreateUser.module.css";

export default class CreateUser extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            user: {},
        };
    }

    createUser() {
        const user = this.state.user
        const data = {
            name: user.name,
            login: user.login,
            password: user.password,
        }

        axios({
            url: "/api/users/create",
            method: "POST",
            headers: {
                authorization: this.props.user.token
            },
            data
        })
            .then((response) => {
                this.props.closeMaster();
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    isUserGood(wh) {
        if (wh?.name == null) return false
        if (wh?.login == null) return false
        if (wh?.password == null) return false

        // passed
        return true;
    }

    render() {
        return (
            <div className={styles.mainContent}>
                <div className={styles.form}>
                    <div className={styles.header}>
                        Create new user
                    </div>
                    <div className={styles.inputGroup}>
                        <label>Name:</label>
                        <input type="text" placeholder="Name" value={this.state.user?.name ?? ""} onChange={(event) => this.setState({ user: { ...this.state.user, name: event.target.value } })} />
                    </div>
                    <div className={styles.inputGroup}>
                        <label>Login:</label>
                        <input type="text" placeholder="Login" value={this.state.user?.login ?? ""} onChange={(event) => this.setState({ user: { ...this.state.user, login: event.target.value } })} />
                    </div>
                    <div className={styles.inputGroup}>
                        <label>Password:</label>
                        <input type="password" placeholder="Password" value={this.state.user?.password ?? ""} onChange={(event) => this.setState({ user: { ...this.state.user, password: event.target.value } })} />
                    </div>
                    <div className={styles.submitButton}>
                        <button type="submit" onClick={() => this.createUser()}>Submit</button>
                    </div>
                </div>
            </div >
        );
    }
}