public class InvalidOperations {
    public static void main(String[] a) {
        System.out.println(new MyClass().semantically_incorrect_expressions());
    }
}

class MyClass {
    int i1;
    int i2;
    boolean b1;
    boolean b2;
    int[] ia1;
    int[] ia2;
    ClassDatatest c1;
    ClassDatatest c2;

    public boolean semantically_incorrect_expressions() {
        i1 = i1 + b1;// @error - semantic ('b1' is of wrong type)
        i1 = i1 + ia1;// @error - semantic ('ia1' is of wrong type)
        i1 = i1 + c1;// @error - semantic ('c1' is of wrong type)
        i1 = b1 + b1;// @error - semantic ('b1' is of wrong type)
        i1 = ia1 + ia1;// @error - semantic ('ia1' is of wrong type)
        i1 = ia1 + ia1[0];// @error - semantic ('ia1' is of wrong type)
        i1 = c1 + c1;// @error - semantic ('c1' is of wrong type)
        i1 = c1 && c1;// @error - semantic ('c1' is of wrong type, 'i1' and expression 'c1 && c1' are
                      // of different types)
        i1 = c1 || c1;// @error - semantic
        i1 = c1 < c1;// @error - semantic
        b1 = b1 + b1;// @error - semantic
        b1 = b1 + ia1;// @error - semantic
        b1 = b1 && ia1;// @error - semantic
        b1 = b1 == c1;// @error - semantic
        ia1 = ia1 + ia2;// @error - semantic
        ia1 = ia1 && ia2;// @error - semantic
        b1 = b1 < b2;// @error - semantic
        b1 = !i1;// @error - semantic
        return true;
    }

    public boolean operator_test_work() {
        i1 = i1 + i2;
        i1 = ia1[1] + ia2[2];
        i1 = c1.get_trash()[2] + i2;
        b1 = b1 && b2;
        b1 = i1 < i2;
        return true;
    }

}

class ClassDatatest {
    int[] trash;

    public int[] get_trash() {
        return trash;
    }
}
