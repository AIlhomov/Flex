public class StatementAfterReturn {
	public static void main(String[] args) {
		System.out.println(new TestClass().getValue());
	}
}

class TestClass {
	public int getValue() {
		int value;
		value = 10;
		return value;
		System.out.println(value); // @error - syntax (Statement after return)
	}
}
