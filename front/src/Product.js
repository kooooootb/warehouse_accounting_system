import React from "react";
import axios from "axios";
import { Navigate } from "react-router-dom";

import styles from "./Product.module.css";

export default class Product extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            product: {},
            operations: [],
            isViewingOperations: false,
            operationsBatchSize: 5,
            noMoreOperations: false,
        };

        this.props.setViewName("Product");
    }

    componentDidMount() {
        axios.get("/api/product", {
            headers: {
                authorization: this.props.user.token
            },
            params: {
                id: this.props.currentProduct,
            }
        })
            .then((response) => {
                const product = response.data.result;
                this.setState({ product });
                this.props.setViewName(`Product ${product.pretty_name}`);
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/product");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    loadNextOperations() {
        if (this.state.noMoreOperations === true) {
            return;
        }

        axios({
            url: "/api/operations",
            method: "POST",
            headers: {
                authorization: this.props.user.token
            },
            data: {
                limit: this.state.operationsBatchSize,
                offset: this.state.operations.length,
                filters: [{ type: "=", key: "product_id", value: this.props.currentProduct }]
            }
        })
            .then((response) => {
                const operations = response.data.result;
                if (operations.length < this.state.operationsBatchSize) {
                    this.setState({ noMoreOperations: true });
                }
                this.setState({ operations: this.state.operations.concat(operations) });
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/product");
                    this.setState({ redirectTo: "/login" });
                }
                else if (error.status === 404) {
                    this.setState({ noMoreOperations: true });
                }
                else console.error(error);
            });

    }

    viewOperations() {
        this.loadNextOperations();
        this.setState({ isViewingOperations: true })
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
                    <div className={styles.productInfo}>
                        <div className={styles.infoHeader}>
                            <p>This product info:</p>
                            <div className={styles.toWarehouse} onClick={() => this.setState({ redirectTo: "/warehouse" })} >
                                To warehouse {this.props.currentWarehouse}
                            </div>
                        </div>
                        <table>
                            <tbody>
                                <tr>
                                    <td><p>id: {this.state.product.product_id}</p></td>
                                    <td><p>name: {this.state.product.name}</p></td>
                                </tr>
                                <tr>
                                    <td><p>pretty name: {this.state.product.pretty_name}</p></td>
                                    <td><p>description: {this.state.product.description}</p></td>
                                </tr>
                                <tr>
                                    <td><p>created in {this.state.product.created_date}</p></td>
                                    <td><p>created by user with id: {this.state.product.created_by}</p></td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    {this.state.isViewingOperations ?
                        <div className={styles.operationList}>
                            {this.state.operations.map((op) => <div key={op.operation_id} className={styles.operationEntry} >
                                <table>
                                    <tbody>
                                        <tr>
                                            <td><p>invoice_id: {op.invoice_id}</p></td>
                                            <td><p>invoice_pretty_name: {op.invoice_pretty_name}</p></td>
                                        </tr>
                                        <tr>
                                            <td><p>{op.warehouse_id > 0 ? "was moved to warehouse: " : "was moved from warehouse: "}{Math.abs(op.warehouse_id)}</p></td>
                                            <td><p>count: {op.count}</p></td>
                                        </tr>
                                        <tr>
                                            <td><p>created_by: {op.created_by}</p></td>
                                            <td><p>created_date: {op.created_date}</p></td>
                                        </tr>
                                    </tbody>
                                </table>
                            </div>)
                            }
                            {this.state.noMoreOperations ? null : <div className={styles.loadMoreButton} onClick={() => this.loadNextOperations()}>
                                Load more operations
                            </div>}
                        </div> :
                        <div className={styles.loadOperationsButton} onClick={() => this.viewOperations()}>
                            Click to load operations
                        </div>
                    }
                </div>
            );
    }
}