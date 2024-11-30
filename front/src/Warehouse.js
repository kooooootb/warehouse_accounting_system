import React from "react";
import axios from "axios";
import { Navigate } from "react-router-dom";

import styles from "./Warehouse.module.css";

export default class Warehouse extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            warehouse: {},
            products: [],
            currentPage: 0,
            maxPage: 0,
            pagesize: 6,
        };

        this.props.setViewName("Warehouse");
    }

    loadProducts() {
        axios({
            url: "/api/products",
            method: "POST",
            headers: {
                authorization: this.props.user.token
            },
            data: {
                limit: this.state.pagesize,
                offset: this.state.currentPage * this.state.pagesize,
                filters: [{ type: "=", key: "warehouse_id", value: this.props.currentWarehouse }]
            }
        })
            .then((response) => {
                const products = response.data.result;
                this.setState({ products });
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouse");
                    this.setState({ redirectTo: "/login" });
                }
                if (error.status != 404) console.error(error);
            });
    }

    loadSize() {
        axios({
            url: "/api/productsizebywarehouse",
            method: "GET",
            headers: {
                authorization: this.props.user.token
            },
            params: {
                warehouse_id: this.props.currentWarehouse
            }
        })
            .then((response) => {
                const maxPage = Math.ceil(response.data.count / this.state.pagesize) - 1;
                this.setState({ maxPage }, () => this.loadProducts());
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouse");
                    this.setState({ redirectTo: "/login" });
                }
            });
    }

    componentDidMount() {
        axios.get("/api/warehouse", {
            headers: {
                authorization: this.props.user.token
            },
            params: {
                id: this.props.currentWarehouse,
            }
        })
            .then((response) => {
                const warehouse = response.data.result;
                this.setState({ warehouse });
                this.props.setViewName(`Warehouse ${warehouse.pretty_name}`);
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouse");
                    this.setState({ redirectTo: "/login" });
                }
                if (error.status != 404) console.error(error);
            });

        this.loadSize();
    }

    chooseProduct(product) {
        this.props.setCurrentProduct(product.product_id);
        this.setState({ redirectTo: "/product" });
    }

    switchPage(switchBy) {
        const newPage = this.state.currentPage + switchBy;
        if (0 <= newPage && newPage <= this.state.maxPage) {
            this.setState({ currentPage: newPage }, () => this.loadProducts());
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
                    <div className={styles.warehouseInfo}>
                        <p>This warehouse info:</p>
                        <table>
                            <tbody>
                                <tr>
                                    <td><p>id: {this.state.warehouse.warehouse_id}</p></td>
                                    <td><p>name: {this.state.warehouse.name}</p></td>
                                </tr>
                                <tr>
                                    <td><p>pretty name: {this.state.warehouse.pretty_name}</p></td>
                                    <td><p>description: {this.state.warehouse.description}</p></td>
                                </tr>
                                <tr>
                                    <td><p>created in {this.state.warehouse.created_date}</p></td>
                                    <td><p>created by user with id: {this.state.warehouse.created_by}</p></td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    {/* <div>
                        {this.state.products.map((pr) => <div key={pr.product_id} className={styles.productEntry} onClick={() => this.chooseProduct(pr)}>{pr.name}</div>)}
                    </div> */}
                    <div className={styles.productsContent}>
                        <p>Products list:</p>
                        <p>Page number: {this.state.currentPage + 1}</p>
                        <div className={styles.productList}>
                            {this.state.products.map((pr) => <div key={pr.product_id} className={styles.productEntry} onClick={() => this.chooseProduct(pr)}>
                                <p>id: {pr.product_id}</p>
                                <p>name: {pr.name}</p>
                                <p>pretty name: {pr.pretty_name}</p>
                                <p>description: {pr.description}</p>
                                <p>created in {pr.created_date}</p>
                                <p>created by user with id: {pr.created_by}</p>
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
                </div>
            );
    }
}