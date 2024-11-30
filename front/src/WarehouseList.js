import React from "react";
import axios from "axios";
import { Navigate } from "react-router-dom";

import styles from "./WarehouseList.module.css"

export default class WarehouseList extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            warehouses: [],
            currentPage: 0,
            maxPage: 0,
            pagesize: 6,
        };

        this.props.setViewName("Warehouses");
    }

    loadWarehouses() {
        axios.get("/api/warehouses", {
            headers: {
                authorization: this.props.user.token
            },
            params: {
                limit: this.state.pagesize,
                offset: this.state.currentPage * this.state.pagesize
            }
        })
            .then((response) => {
                const warehouses = response.data.result;
                this.setState({ warehouses });
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouses");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    loadSize() {
        axios.get("/api/warehousesize", {
            headers: {
                authorization: this.props.user.token
            }
        })
            .then((response) => {
                const maxPage = Math.ceil(response.data.count / this.state.pagesize) - 1;
                this.setState({ maxPage }, () => this.loadWarehouses());
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouses");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    componentDidMount() {
        this.loadSize();
    }

    chooseWarehouse(warehouse) {
        this.props.setCurrentWarehouse(warehouse.warehouse_id);
        this.setState({ redirectTo: "/warehouse" });
    }

    switchPage(switchBy) {
        const newPage = this.state.currentPage + switchBy;
        if (0 <= newPage && newPage <= this.state.maxPage) {
            this.setState({ currentPage: newPage }, () => this.loadWarehouses());
        }
    }

    render() {
        if (this.state?.redirectTo) {
            return (
                <Navigate to={this.state.redirectTo} />
            )
        }
        else
            return (
                <div className={styles.mainContent}>
                    <p>Warehouses list:</p>
                    <p>Page number: {this.state.currentPage + 1}</p>
                    <div className={styles.warehouseList}>
                        {this.state.warehouses.map((wh) => <div key={wh.warehouse_id} className={styles.warehouseEntry} onClick={() => this.chooseWarehouse(wh)}>
                            <p>id: {wh.warehouse_id}</p>
                            <p>name: {wh.name}</p>
                            <p>pretty name: {wh.pretty_name}</p>
                            <p>description: {wh.description}</p>
                            <p>created in {wh.created_date}</p>
                            <p>created by user with id: {wh.created_by}</p>
                        </div>)}
                    </div>
                    <div className={styles.pageDiv}>
                        <div className={styles.pageList}>
                            <div className={styles.pageEntry} onClick={() => this.switchPage(-1)}>
                                <p className={styles.pageText}>&lt;</p>
                                <p className={styles.pageText}>{this.state.currentPage - 1 >= 0 ? this.state.currentPage : ""}</p>
                            </div>
                            <div className={styles.pageEntry} style={{ "backgroundColor": "lightblue" }}>
                                <p className={styles.pageText}>current</p>
                                <p className={styles.pageText}>{this.state.currentPage + 1}</p>
                            </div>
                            <div className={styles.pageEntry} onClick={() => this.switchPage(1)}>
                                <p className={styles.pageText}>&gt;</p>
                                <p className={styles.pageText}>{this.state.currentPage + 1 <= this.state.maxPage ? this.state.currentPage + 2 : ""}</p>
                            </div>
                        </div>
                    </div>
                </div>
            );
    }
}