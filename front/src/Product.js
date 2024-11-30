import React from "react";
import axios from "axios";
import { Navigate } from "react-router-dom";

import styles from "./Product.module.css";

export default class Product extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            product: {},
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

    render() {
        if (this.state?.redirectTo) {
            return (
                <Navigate to={this.state.redirectTo} />
            )
        }
        else
            return (
                <div className={styles.mainContent}>
                    <div className={styles.toWarehouse} onClick={() => this.setState({ redirectTo: "/warehouse" })} >
                        To warehouse
                    </div>
                    <div>
                        <p>created_by: {this.state.product.created_by}</p>
                        <p>created_date: {this.state.product.created_date}</p>
                        <p>description: {this.state.product.description}</p>
                        <p>main_color: {this.state.product.main_color}</p>
                        <p>name: {this.state.product.name}</p>
                        <p>pretty_name: {this.state.product.pretty_name}</p>
                        <p>product_id: {this.state.product.product_id}</p>
                    </div>
                </div>
            );
    }
}