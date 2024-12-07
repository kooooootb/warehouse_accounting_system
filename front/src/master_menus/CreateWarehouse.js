import React from "react";
import axios from "axios";

import styles from "./CreateWarehouse.module.css";

export default class CreateWarehouse extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            warehouse: {},
        };
    }

    createWarehouse() {
        const warehouse = this.state.warehouse

        if (!this.isWarehouseGood(warehouse)) {
            alert("Required fields not filled")
            return;
        }

        const data = {
            name: warehouse.name,
            ...(warehouse.description && { description: warehouse.description }),
            ...(warehouse.location && { location: warehouse.location }),
        }

        axios({
            url: "/api/warehouses/create",
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

    isWarehouseGood(wh) {
        if (wh?.name == null) return false

        // passed
        return true;
    }

    render() {
        return (
            <div className={styles.mainContent}>
                <div className={styles.form}>
                    <div className={styles.header}>
                        Create new warehouse
                    </div>
                    <div className={styles.inputGroup}>
                        <label>Warehouse name:</label>
                        <input type="text" placeholder="Name" value={this.state.warehouse?.name ?? ""} onChange={(event) => this.setState({ warehouse: { ...this.state.warehouse, name: event.target.value } })} />
                    </div>
                    <div className={styles.textareaGroup}>
                        <label>Warehouse description:</label>
                        <textarea type="text" placeholder="Description" value={this.state.warehouse?.description ?? ""} onChange={(event) => this.setState({ warehouse: { ...this.state.warehouse, description: event.target.value } })} />
                    </div>
                    <div className={styles.textareaGroup}>
                        <label>Warehouse location:</label>
                        <textarea type="text" placeholder="Location" value={this.state.warehouse?.location ?? ""} onChange={(event) => this.setState({ warehouse: { ...this.state.warehouse, location: event.target.value } })} />
                    </div>
                    <div className={styles.submitButton}>
                        <button type="submit" onClick={() => this.createWarehouse()}>Submit</button>
                    </div>
                </div>
            </div >
        );
    }
}