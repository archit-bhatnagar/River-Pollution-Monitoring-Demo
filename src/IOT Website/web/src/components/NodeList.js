import React from 'react';

import Node from './Node';
import classes from './NodeList.module.css';

const NodeList = (props) => {
  return (
    <ul className={classes['node-list']}>
      {props.nodes.map((node) => (
        <Node
          key={node.id}
          humidity={node.humidity}
          temperature={node.temperature}
          distance={node.distance}
          title={node.title}
        />
      ))}
    </ul>
  );
};

export default NodeList;
