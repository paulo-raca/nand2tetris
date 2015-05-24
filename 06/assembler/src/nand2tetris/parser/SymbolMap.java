package nand2tetris.parser;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Properties;


@SuppressWarnings("serial")
public class SymbolMap extends LinkedHashMap<String, Integer> {
	String symbolName;
	public SymbolMap(String symbolName, String file) {
		this(symbolName, file, 10);
	}
	public SymbolMap(String symbolName, String file, int radix) {
		try {
			this.symbolName = symbolName;
			Properties properties = new Properties();
			properties.load(getClass().getResourceAsStream(file));
			for (Object key : properties.keySet()) {
				put(key.toString(), Integer.parseInt(properties.get(key).toString(), radix));
			}
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
	}
	
	public Integer get(String symbol) {
		if (!this.containsKey(symbol)) {
			throw new IllegalArgumentException("Invalid " + symbolName + ": " + symbol);
		}
		return super.get(symbol);
	}
}
