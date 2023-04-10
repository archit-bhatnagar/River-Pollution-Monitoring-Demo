import React, { useState, useEffect, useCallback } from "react";

import NodeList from "./components/NodeList";
import "./App.css";

function App() {
  const [sensorData, setSensorData] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);

  const fetchSensorDataHandler = useCallback(async () => {
    setIsLoading(true);
    setError(null);
    try {
      var response = await fetch("https://fir-esp8266-f80a8-default-rtdb.asia-southeast1.firebasedatabase.app/master.json");
      if (!response.ok) {
        throw new Error("Something went wrong!");
      }
      var data = await response.json();
      console.log("data= ", data);

      const loadedData = [];

      loadedData.push({
        id: 1,
        humidity: data.humidity,
        temperature: data.temperature,
        distance: data.distance,
        title: "Node No. 1",
      });

      // add data for second node as well
      const response2 = await fetch("https://fir-esp8266-f80a8-default-rtdb.asia-southeast1.firebasedatabase.app/slave.json");
      const data2 = await response2.json();
      console.log("data2= ", data2);

      loadedData.push({
        id: 2,
        humidity: data2.humidity,
        temperature: data2.temperature,
        distance: data2.distance,
        title: "Node No. 2",
      });

      // show difference as well
      loadedData.push({
        id: 3,
        humidity: data.humidity - data2.humidity,
        temperature: data.temperature - data2.temperature,
        distance: data.distance - data2.distance,
        title: "Difference",
      })

      setSensorData(loadedData);
    } catch (error) {
      setError(error.message);
    }
    setIsLoading(false);
  }, []);

  useEffect(() => {
    fetchSensorDataHandler();
  }, [fetchSensorDataHandler]);


  const MINUTE_MS = 10000;

  useEffect(() => {
    const interval = setInterval(() => {
      console.log('Logs every 10s');
      fetchSensorDataHandler();
    }, MINUTE_MS);

  return () => clearInterval(interval); // This represents the unmount function, in which you need to clear your interval to prevent memory leaks.
  }, [fetchSensorDataHandler])

  let content = <p>Found no data :(</p>;

  if (sensorData.length > 0)
    content = <NodeList nodes={sensorData} />;

  if (error)
    content = <p>{error}</p>;

  if (isLoading)
    content = <p>Loading...</p>;

  return (
    <React.Fragment>
      <h1>WATER POLLUTION MONITORING</h1>
      <section>
        <button onClick={fetchSensorDataHandler}>Fetch data</button>
      </section>
      <section>{content}</section>
    </React.Fragment>
  );
}

export default App;
