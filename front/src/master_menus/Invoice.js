import React from "react";
import axios from "axios";

import styles from "./Invoice.module.css";
import ProductEdit from "./ProductEdit.js";

export default class InvoiceMaster extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            type: "move",
            warehouses: [],
            products: [],
            warehouse_from_id: undefined,
            warehouse_to_id: undefined,
            currentProductIndex: null,
            invoice: {},
        };
    }

    componentDidMount() {
        this.loadWarehouses();
    }

    loadWarehouses() {
        axios.get("/api/warehouses", {
            headers: {
                authorization: this.props.user.token
            },
            params: {
                limit: 0,
                offset: 0
            }
        })
            .then((response) => {
                const warehouses = response.data.result.map(({ warehouse_id, pretty_name }) => ({ warehouse_id, pretty_name }));
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

    selectType(newType) {
        this.setState({ type: newType, products: [] });
    }

    handleProductEdit(editProduct) {
        if (editProduct != null) {
            // saving
            let newProducts;

            if (this.state.currentProductIndex === this.state.products.length) {
                // adding new
                newProducts = [...this.state.products, editProduct]
            }
            else {
                // changing existing
                newProducts = this.state.products.map((pr, prIndex) => prIndex === this.state.currentProductIndex ? editProduct : pr)
            }

            this.setState({
                products: newProducts,
                currentProductIndex: null
            })
        }
        else {
            // discarding
            this.setState({ currentProductIndex: null })
        }
    }

    isInvoiceGood(inv) {
        // check name
        if (inv?.invoice_name == null) return false

        // check warehouses
        switch (this.state.type) {
            case "new": if (this.state.warehouse_to_id == null) return false; break;
            case "move": if (this.state.warehouse_to_id == null || this.state.warehouse_from_id == null) return false; break;
            case "remove": if (this.state.warehouse_from_id == null) return false; break;
            default: console.error(`wrong type: ${this.state.type}`)
        }

        // check products
        if (this.state.products?.length === 0) return false

        // passed
        return true;
    }

    saveInvoice() {
        if (!this.isInvoiceGood(this.state.invoice)) {
            console.error(this.state.invoice)
            console.error(`tried to save bad invoice`)
            return;
        }

        const products = this.state.products.map((pr) => {
            if (pr?.product_id != null) {
                return { id: pr.product_id, count: pr.count }
            }
            else {
                return pr
            }
        })

        let data = {
            invoice_name: this.state.invoice.invoice_name,
            ...(this.state.invoice.invoice_description && { invoice_description: this.state.invoice.invoice_description }),
            products
        }
        let url = ""

        switch (this.state.type) {
            case "new":
                data = { ...data, warehouse_to: this.state.warehouse_to_id }
                url = "/api/products/create"
                break
            case "move":
                data = { ...data, warehouse_to: this.state.warehouse_to_id, warehouse_from: this.state.warehouse_from_id }
                url = "/api/products/move"
                break
            case "remove":
                data = { ...data, warehouse_from: this.state.warehouse_from_id }
                url = "/api/products/remove"
                break
            default: console.error(`wrong type: ${this.state.type}`)
        }

        axios({
            url,
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
                    this.props.setNextRedirect("/warehouses");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    render() {
        if (this.state.currentProductIndex !== null)
            return (<ProductEdit user={this.props.user} forceById={this.state.type !== "new"} warehouse_id={this.state.type === "new" ? null : this.state.warehouse_from_id} product={this.state.products[this.state.currentProductIndex] ?? null} setter={(newPr) => this.handleProductEdit(newPr)} />)
        else
            return (
                <div className={styles.mainContent}>
                    <div className={styles.typeSwitchHeader}>
                        <div className={this.state.type === "new" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("new")}>
                            New products
                        </div>
                        <div className={this.state.type === "move" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("move")}>
                            Move products between warehouse
                        </div>
                        <div className={this.state.type === "remove" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("remove")}>
                            Remove products from warehouse
                        </div>
                    </div>
                    <div className={styles.invoiceForm}>
                        <div className={styles.inputGroup}>
                            <label>Invoice name:</label>
                            <input type="text" placeholder="Name" value={this.state.invoice?.invoice_name ?? ""} onChange={(event) => this.setState({ invoice: { ...this.state.invoice, invoice_name: event.target.value } })} />
                        </div>
                        <div className={styles.textareaGroup}>
                            <label>Invoice description:</label>
                            <textarea type="text" placeholder="Description" value={this.state.invoice?.invoice_description ?? ""} onChange={(event) => this.setState({ invoice: { ...this.state.invoice, invoice_description: event.target.value } })} />
                        </div>
                        <div className={styles.warehousesGroup}>
                            <div className={styles.warehouseSelect}>
                                <label>Warehouse from: </label>
                                <select value={this.state.warehouse_from_id} name="warehouse_from_id" disabled={this.state.type === "new"} onChange={(event) => { this.setState({ warehouse_from_id: event.target.value === "" ? undefined : Number(event.target.value), products: [] }) }}>
                                    <option value={""}>choose warehouse</option>
                                    {this.state.warehouses.map((wh) => (
                                        <option key={wh.warehouse_id} value={wh.warehouse_id}>
                                            {wh.pretty_name}
                                        </option>
                                    ))}
                                </select>
                            </div>
                            <div className={styles.warehouseSelect}>
                                <label>Warehouse to: </label>
                                <select value={this.state.warehouse_to_id} name="warehouse_to_id" disabled={this.state.type === "remove"} onChange={(event) => { this.setState({ warehouse_to_id: event.target.value === "" ? undefined : Number(event.target.value) }) }}>
                                    <option value={""}>choose warehouse</option>
                                    {this.state.warehouses.map((wh) => (
                                        <option key={wh.warehouse_id} value={wh.warehouse_id}>
                                            {wh.pretty_name}
                                        </option>
                                    ))}
                                </select>
                            </div>
                        </div>
                        <div className={styles.productMenu}>
                            <div className={styles.productAddButton} onClick={() => { if (this.state.type === "new" || this.state.warehouse_from_id != null) this.setState({ currentProductIndex: this.state.products.length }) }} >
                                Add product
                            </div>
                            {this.state.products.length > 0 ?
                                <div className={styles.productList} >
                                    {this.state.products.map((pr, prIndex) => (
                                        <div key={prIndex} className={styles.productEntry} onClick={() => this.setState({ currentProductIndex: prIndex })}>
                                            <div className={styles.productInfo}>
                                                name: {pr.name}<br />count: {pr.count}
                                            </div>
                                            <div className={styles.productRemove} onClick={(event) => {
                                                event.stopPropagation()
                                                this.setState({
                                                    products: this.state.products.filter((item, index) => index !== prIndex)
                                                })
                                            }}>
                                                remove
                                            </div>
                                        </div>
                                    ))}
                                </div> :
                                null}
                        </div>
                        <div className={styles.submitButton}>
                            <button type="submit" onClick={() => this.saveInvoice()}>Submit</button>
                        </div>
                    </div>
                </div >
            );
    }
}