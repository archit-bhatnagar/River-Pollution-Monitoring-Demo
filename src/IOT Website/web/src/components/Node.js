import React from 'react';

import classes from './Node.module.css';

const Node = (props) => {
  return (
    <li className={classes.node}>
      <h2>{props.title}</h2>
      <h3>Temperature: {props.temperature} °C</h3>
      <p>Humidity: {props.humidity} %</p>
      <p>pH:{props.distance} cm</p>
    </li>
  );
};

export default Node;
