public class SemanticDuplicateIdentifiersSeparateScope {
    public static void main(String[] a) {
        System.out.println(new A().A());
    }
}

class A {
    int A;

    public int A() {
        int A;
        System.out.println(this.B());
        A = 1;
        return A;
    }

    public int B() {
        int B;
        A = 5;
        B = new B().B();
        return A + B;
    }
}

class B {
    int A;

    public int A(int A) {
        return A;
    }

    public int B() {
        int B;
        B = 1;
        return B;
    }
}