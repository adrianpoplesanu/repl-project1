package com.ad.objects;

import com.ad.ast.AstNode;

import java.util.List;

public class AdClassObject extends AdObject {
    private AstNode klassNode;
    private AstNode name;
    private List<AstNode> methods;
    private List<AstNode> attributes;

    public AdClassObject() {
        type = ObjectTypeEnum.CLASS;
    }

    @Override
    public String inspect() {
        return "<class at memory address: " + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public void print() {
        System.out.println("Class object");
    }

    @Override
    public String hash() {
        return ObjectTypeConverter.convertToString(type) + inspect();
    }

    public AstNode getKlassNode() {
        return klassNode;
    }

    public void setKlassNode(AstNode klassNode) {
        this.klassNode = klassNode;
    }

    public List<AstNode> getMethods() {
        return methods;
    }

    public void setMethods(List<AstNode> methods) {
        this.methods = methods;
    }

    public List<AstNode> getAttributes() {
        return attributes;
    }

    public void setAttributes(List<AstNode> attributes) {
        this.attributes = attributes;
    }

    public AstNode getName() {
        return name;
    }

    public void setName(AstNode name) {
        this.name = name;
    }
}
