public class SemanticArrayInteger {
    public static void main(String[] a) {
        System.out.println(new A().a2().length);
    }
}

class A {

    public A a1(int num) {
        int[] num_aux;
        num_aux[0] = 2;

        if (num_aux[0] < 1)
            num_aux[0] = 1;
        else
            num_aux = this.a2();

        num_aux[3] = num_aux.length;
        return this;
    }

    public int[] a2() {
        int[] a;
        a[0] = 2;
        return a;
    }
}
