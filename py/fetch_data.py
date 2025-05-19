import databento as db
from dotenv import load_dotenv
from os import getenv
from datetime import datetime,timedelta
load_dotenv()

API_KEY = getenv("API_KEY")
DATASET = "XNAS.ITCH"
SYMBOLS = ["SPY"]
END   = datetime.now().date()
START = END - timedelta(days=30)
OUTPUT_FILE = "ticks.dbn"

client = db.Historical(key=API_KEY)
data = client.timeseries.get_range(
    dataset=DATASET,
    symbols=SYMBOLS,
    start=START,
    end=END,
    schema=db.Schema.TRADES,         # Trade messages
)
data.to_file(OUTPUT_FILE)
    

print("DBN tick data written to", OUTPUT_FILE)