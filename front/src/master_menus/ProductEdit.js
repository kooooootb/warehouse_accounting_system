import React from "react";
import axios from "axios";

import styles from "./ProductEdit.module.css";

export default class ProductEdit extends React.Component {
    constructor(props) {
        super(props);

        const type = (props.forceById || props.product?.product_id) ? "byid" : "new";

        if (type === "byid") { this.loadProducts() }

        this.state = {
            type,
            product: type === "new" ? props.product : {},
            selectedProduct: type === "byid" ? props.product : {},
            products: [],
            maxCount: null,
        };
    }

    selectType(newType) {
        if (this.props.forceById) { console.error("tried to switch type while forcing byid"); return }

        this.setState({ type: newType });

        if (newType === "byid" && this.state.products.length === 0) {
            this.loadProducts();
        }
    }

    loadProducts() {
        let filters = null
        if (this.props?.warehouse_id) {
            filters = [{ type: "=", key: "warehouse_id", value: this.props.warehouse_id }]
        }

        axios({
            url: "/api/products",
            method: "POST",
            headers: {
                authorization: this.props.user.token
            },
            data: {
                ...(filters && { filters }),
                limit: 0,
                offset: 0
            }
        })
            .then((response) => {
                const products = response.data.result;

                let maxCount = null;
                const selectedPrId = this.state.selectedProduct?.product_id
                if (selectedPrId != null) {
                    maxCount = products.find((pr) => selectedPrId === pr.product_id).count;
                }
                this.setState({ ...(maxCount && { maxCount }), products });
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouses");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    isProductGood = (pr) => {
        return (pr?.product_id != null || pr?.name != null) && (pr?.count && pr.count > 0 && (this.state.maxCount == null || pr.count <= this.state.maxCount));
    }

    saveProduct() {
        const product = this.state.type === "new" ? this.state.product : this.state.selectedProduct;

        if (this.isProductGood(product)) {
            this.props.setter(product)
        }
        else {
            console.error(product)
            console.error(`tried to save bad product ${product}`)
        }
    }

    discardProduct() {
        this.props.setter(this.props.product)
    }

    selectProduct(prId) {
        const product = this.state.products.find((pr) => pr.product_id === prId);
        this.setState({ selectedProduct: product, maxCount: product.count });
    }

    render() {
        return (
            <div className={styles.mainContent}>
                <div className={styles.typeSwitchHeader}>
                    <div className={this.state.type === "byid" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("byid")}>
                        Existing product
                    </div>
                    <div className={this.props.forceById ? styles.hidden : (this.state.type === "new" ? styles.selectedTypeSwitcher : styles.typeSwitcher)} onClick={() => this.selectType("new")}>
                        New product
                    </div>
                </div>
                {this.state.type === "new" ?
                    <div className={styles.productForm}>
                        <div className={styles.inputGroup}>
                            <label>Product name:</label>
                            <input type="text" placeholder="Name" value={this.state.product?.name ?? ""} onChange={(event) => this.setState({ product: { ...this.state.product, name: event.target.value } })} />
                        </div>
                        <div className={styles.textareaGroup}>
                            <label>Product description:</label>
                            <textarea type="text" placeholder="Description" value={this.state.product?.description ?? ""} onChange={(event) => this.setState({ product: { ...this.state.product, description: event.target.value } })} />
                        </div>
                        <div className={styles.inputGroup}>
                            <label>Product's count:</label>
                            <input type="number" placeholder="1" value={this.state.product?.count ?? ""} onChange={(event) => this.setState({ product: { ...this.state.product, count: Number(event.target.value) } })} />
                        </div>
                    </div> :
                    <div className={styles.productForm}>
                        <div className={styles.productSelect}>
                            <label>Select product: </label>
                            <select value={this.state.selectedProduct?.product_id} onChange={(event) => { event.target.value !== null && this.selectProduct(Number(event.target.value)) }}>
                                <option value={null}>choose product</option>
                                {this.state.products.map((pr) => (
                                    <option key={pr.product_id} value={pr.product_id}>
                                        {pr.pretty_name}
                                    </option>
                                ))}
                            </select>
                        </div>
                        <div className={styles.inputGroup}>
                            <label>Product's count (maximum {this.state.maxCount}):</label>
                            <input type="number" placeholder="1" value={this.state.selectedProduct?.count ?? ""} onChange={(event) => this.setState({ selectedProduct: { ...this.state.selectedProduct, count: Number(event.target.value) } })} />
                        </div>
                        <div className={styles.productInfo}>
                            <p>id: {this.state.selectedProduct?.product_id}</p>
                            <p>name: {this.state.selectedProduct?.name}</p>
                            <p>pretty name: {this.state.selectedProduct?.pretty_name}</p>
                            <p>description: {this.state.selectedProduct?.description}</p>
                            <p>created in {this.state.selectedProduct?.created_date}</p>
                            <p>created by user with id: {this.state.selectedProduct?.created_by}</p>
                        </div>
                    </div>
                }
                <div className={styles.saveButton} onClick={() => this.saveProduct()}>
                    <button>Save</button>
                </div>
                <div className={styles.discardButton} onClick={() => this.discardProduct()}>
                    <button>Discard</button>
                </div>
            </div>
        );
    }
}