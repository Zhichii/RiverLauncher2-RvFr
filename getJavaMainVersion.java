// Modified from https://blog.csdn.net/HaHa_Sir/article/details/84984234
public class getJavaMainVersion {
    public static void main(String[] args) {
        System.out.println(System.getProperty("java.specification.version"));
        System.out.println(System.getProperty("sun.arch.data.model"));
        System.out.println(System.getProperty("java.version"));
        System.out.println(System.getProperty("os.arch"));
        System.out.println(System.getProperty("java.vm.name"));
    }
}